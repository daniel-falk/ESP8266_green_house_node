#ifndef __SLAVE_H
#define __SLAVE_H

enum slave_type
{
  TYPE_UNKNOWN = 0,
  TYPE_BME280
};

struct slave {
  void *object;
  enum slave_type type;
  String description;
};

#endif
