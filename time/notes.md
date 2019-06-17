# Time Notes

* string return by ctime is statically allocated multiple calls will
overwrite it.

* mktime may modify the provided struct tm* provided

* strptime never sets tm_isdst
