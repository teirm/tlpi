/*
 * my_strerror.h
 *
 * contains threads safe and thread un-safe implementations
 */

/*
 * @brief thread unsafe implementation
 *
 * @param[in] int err   error number
 *
 * @return char *
 */
char *my_strerror(int err);

/*
 * @brief thread safe implemation of strerror
 *
 * @param[in] int err   error number
 *
 * @return char *
 */
char *my_strerror_tsd(int err);
