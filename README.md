# error-codes - lookup error codes and descriptions

This command is inspired by the errno command from moreutils. It got
enhanced to lookup error codes for errno, libeconf and pam and
display the name of the error, the value and the description.

```
# error-codes errno EBUSY ENOENT 22
EBUSY - 16 - Device or resource busy
ENOENT - 2 - No such file or directory
EINVAL - 22 - Invalid argument
# error-codes econf ECONF_NOKEY 22
ECONF_NOKEY - 5 - Key not found
ECONF_ARGUMENT_IS_NULL_VALUE - 22 - Given argument is NULL
```

Additionally, it is possible to search for words in the error description.
This can even be done for all locales.

```
# error-codes pam -s token
PAM_NEW_AUTHTOK_REQD - 12 - Authentication token is no longer valid; new one required
PAM_AUTHTOK_ERR - 20 - Authentication token manipulation error
PAM_AUTHTOK_LOCK_BUSY - 22 - Authentication token lock busy
PAM_AUTHTOK_DISABLE_AGING - 23 - Authentication token aging disabled
PAM_AUTHTOK_EXPIRED - 27 - Authentication token expired
# error-codes pam -s token no
PAM_NEW_AUTHTOK_REQD - 12 - Authentication token is no longer valid; new one required
# error-codes errno -s busy
EBUSY - 16 - Device or resource busy
ETXTBSY - 26 - Text file busy
```

