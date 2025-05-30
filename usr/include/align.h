// usr/include/align.h
#ifndef ALIGN_H
#define ALIGN_H

#define ALIGN_MASK(x, mask)    (((x) + (mask)) & ~(mask))
#define ALIGN_UP(x, align)     ALIGN_MASK(x, (typeof(x))(align) - 1)
#define ALIGN_DOWN(x, align)   ((x) & ~((typeof(x))(align) - 1))
#define IS_ALIGNED(x, align)   (((x) & ((typeof(x))(align) - 1)) == 0)

#endif