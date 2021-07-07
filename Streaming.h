#ifndef STREAMING_H
#define STREAMING_H

template<class T>
Print& operator<<(Print &obj, T arg) {
    obj.print(arg);
    return obj;
} 

#endif
