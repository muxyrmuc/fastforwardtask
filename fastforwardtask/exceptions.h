#ifndef FASTFORWARDTASK_EXCEPTIONS_H_
#define FASTFORWARDTASK_EXCEPTIONS_H_

#include <stdexcept>

class DeserializationError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class JsonDecodeError : public DeserializationError {
    using DeserializationError::DeserializationError;
};

class KeyError : public DeserializationError {
    using DeserializationError::DeserializationError;
};

class ValueError : public DeserializationError {
    using DeserializationError::DeserializationError;
};

class SerializationError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class JsonEncodeError : public SerializationError {
    using SerializationError::SerializationError;
};

#endif  // FASTFORWARDTASK_EXCEPTIONS_H_
