
message(STATUS  "[Patch] Target file: ${TARGET_FILE}")
file(READ ${TARGET_FILE} file_content)

string(REPLACE "VERSION 3.0" "VERSION 3.10" new_content "${file_content}")

file(WRITE ${TARGET_FILE} "${new_content}")

message(STATUS "Patched ${TARGET_FILE}: Upgraded version requirement to 3.10")
