#include "serializationutils.h"
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <sstream>
#include <cmath>
#include <clocale>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#include "rapidjson/document.h"
#pragma GCC diagnostic pop

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "exceptions.h"
#include "nonnegativefixedprecision.h"

constexpr std::string_view kObjectPrefix = "Get Object: ";
const std::unordered_map<std::string_view, Message::Type> kStringToMessageType = {
    {"snapshot", Message::Type::kSnapshot},
    {"update", Message::Type::kUpdate}
};
constexpr rapidjson::SizeType kLevelArraySize = 2;

namespace {

inline void CheckJsonEncodeResult(bool result) {
    if (!result) {
        throw JsonEncodeError("error appending value");
    }
}

inline rapidjson::GenericStringRef<char> MakeGenericStringRef(std::string_view from) {
    return rapidjson::GenericStringRef<char>(from.data(), from.size());
}

rapidjson::Value::ConstMemberIterator GetConstMemberIterator(const rapidjson::Document& doc, std::string_view key) {
    const rapidjson::Value::ConstMemberIterator field_it = doc.FindMember(MakeGenericStringRef(key));
    if (doc.MemberEnd() == field_it) {
        throw KeyError("no '" + std::string(key) + "' member found");
    }

    return field_it;
}

inline NonNegativeFixedPrecision GetArrayFloat(const rapidjson::Value::ConstArray& doc, rapidjson::SizeType index) {
    const auto& value = doc[index];
    if (!value.IsString()) {
        throw ValueError("value is expected to be convertible to float");
    }

    return NonNegativeFixedPrecision(value.GetString());
}

Message::Type GetMessageType(const rapidjson::Document& doc) {
    const rapidjson::Value::ConstMemberIterator field_it = GetConstMemberIterator(doc, "type");

    if (!field_it->value.IsString()) {
        throw ValueError("invalid 'type' member type");
    }

    const auto type_it = kStringToMessageType.find(field_it->value.GetString());
    if (kStringToMessageType.end() == type_it) {
        throw ValueError("invalid 'type' member value");
    }

    return type_it->second;
}

std::vector<Level> GetLevels(const rapidjson::Document& doc, std::string_view key) {
    const rapidjson::Value::ConstMemberIterator field_it = GetConstMemberIterator(doc, key);

    if (!field_it->value.IsArray()) {
        throw ValueError("'" + std::string(key) + "' member is not an array");
    }

    std::vector<Level> levels;
    levels.reserve(field_it->value.Size());

    for (const auto& level : field_it->value.GetArray()) {
        if (!level.IsArray()) {
            throw ValueError("level is not an array");
        }

        const rapidjson::Value::ConstArray level_arr = level.GetArray();

        if (kLevelArraySize != level_arr.Size()) {
            throw ValueError("invalid level array size");
        }

        levels.push_back({::GetArrayFloat(level_arr, 0), ::GetArrayFloat(level_arr, 1)});
    }

    return levels;
}

std::uint64_t GetEventTimestamp(const rapidjson::Document& doc) {
    const rapidjson::Value::ConstMemberIterator field_it = GetConstMemberIterator(doc, "event_time");

    if (!field_it->value.IsString()) {
        throw ValueError("'event_time' member value should be convertible to uint64");
    }

    try {
        return std::stoull(field_it->value.GetString());
    } catch (const std::logic_error& e) {
        throw ValueError(e.what());
    }
}

}

Message ParseLogLine(std::string_view line) {
    const std::string_view::size_type pfx_idx = line.find(kObjectPrefix);
    if (std::string_view::npos == pfx_idx) {
        throw ValueError("object prefix not found");
    }

    rapidjson::Document parsed_doc;
    parsed_doc.Parse<rapidjson::kParseNumbersAsStringsFlag>(
        ::MakeGenericStringRef(line.substr(pfx_idx + kObjectPrefix.size())));

    if (parsed_doc.HasParseError()) {
        throw JsonDecodeError("failed to parse JSON; error code: " + std::to_string(parsed_doc.GetParseError()));
    }

    if (!parsed_doc.IsObject()) {
        throw ValueError("JSON document is not an object");
    }

    return {
        ::GetLevels(parsed_doc, "bids"),
        ::GetLevels(parsed_doc, "asks"),
        ::GetEventTimestamp(parsed_doc),
        ::GetMessageType(parsed_doc)
    };
}

std::string SaveToJson(std::uint64_t event_ts_mcsec,
                       const std::optional<Level>& best_bid,
                       const std::optional<Level>& best_ask) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

    ::CheckJsonEncodeResult(writer.StartObject());

    ::CheckJsonEncodeResult(writer.String("event_time"));
    ::CheckJsonEncodeResult(writer.Uint64(event_ts_mcsec));

    ::CheckJsonEncodeResult(writer.String("best_bid"));
    ::CheckJsonEncodeResult(writer.StartArray());
    if (best_bid.has_value()) {
        const std::string price_str = best_bid->price.ToString();
        const std::string amount_str = best_bid->amount.ToString();

        ::CheckJsonEncodeResult(writer.RawValue(price_str.data(),
                                                price_str.size(),
                                                rapidjson::kNumberType));
        ::CheckJsonEncodeResult(writer.RawValue(amount_str.data(),
                                                amount_str.size(),
                                                rapidjson::kNumberType));
    }
    ::CheckJsonEncodeResult(writer.EndArray());

    ::CheckJsonEncodeResult(writer.String("best_ask"));
    ::CheckJsonEncodeResult(writer.StartArray());
    if (best_ask.has_value()) {
        const std::string price_str = best_ask->price.ToString();
        const std::string amount_str = best_ask->amount.ToString();

        ::CheckJsonEncodeResult(writer.RawValue(price_str.data(),
                                                price_str.size(),
                                                rapidjson::kNumberType));
        ::CheckJsonEncodeResult(writer.RawValue(amount_str.data(),
                                                amount_str.size(),
                                                rapidjson::kNumberType));
    }
    ::CheckJsonEncodeResult(writer.EndArray());

    ::CheckJsonEncodeResult(writer.EndObject());

    return buffer.GetString();
}
