#ifndef UTIL_H_
#define UTIL_H_

#define HIDE_CONSTRUCTORS(className)    \
private:    \
    className();    \
    className(const className &);   \
    className &operator=(const className &);    \
    className(const className &&);  \
    className &operator=(const className &&);

#define CREATE_DEFAULT_CONSTRUCTORS(className)  \
    className() = default;    \
    className(const className &) = default;   \
    className &operator=(const className &) = default;    \
    className(const className &&) = default;  \
    className &operator=(const className &&) = default;

#define CREATE_DEFAULT_DESTRUCTOR(className)    \
    ~className() = default;

#define DELETE_COPY_AND_MOVE_CONSTRUCTORS(className)    \
    className(const className &) = delete;   \
    className &operator=(const className &) = delete;    \
    className(const className &&) = delete;  \
    className &operator=(const className &&) = delete;


#endif // util.h
