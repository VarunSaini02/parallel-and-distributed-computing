#ifndef rc_h
#define rc_h

typedef enum rc_st {
   Success = 0,
   OutOfMemory = 1,
   InvalidArgument = 2,
   InvalidOperation = 3,
   Error = 4,
   QueueFull = 5,
   QueueEmpty = 6
} rc_t;

#endif
