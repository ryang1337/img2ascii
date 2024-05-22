typedef enum {
  SUCCESS = 0,
  ERR_INVALID_FILE_NAME,
  ERR_NOT_A_PNG,
  ERR_CREATE_READ_STRUCT_FAIL,
  ERR_CREATE_WRITE_STRUCT_FAIL,
  ERR_CREATE_INFO_STRUCT_FAIL,
  ERR_JMP_FAIL,
  ERR_UNKNOWN
} Error;

const char *get_error_message(Error err) {
  switch (err) {
  case SUCCESS:
    return "Success";
  case ERR_INVALID_FILE_NAME:
    return "Invalid file name";
  case ERR_NOT_A_PNG:
    return "Input file is not a PNG file";
  case ERR_CREATE_READ_STRUCT_FAIL:
    return "Could not create read struct";
  case ERR_CREATE_WRITE_STRUCT_FAIL:
    return "Could not create write struct";
  case ERR_CREATE_INFO_STRUCT_FAIL:
    return "Could not create info struct";
  case ERR_JMP_FAIL:
    return "Failure during jmp";
  case ERR_UNKNOWN:
    return "An unknown error occurred";
  }
}
