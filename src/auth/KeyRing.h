// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*- 
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2004-2009 Sage Weil <sage@newdream.net>
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software 
 * Foundation.  See file COPYING.
 * 
 */

#ifndef CEPH_KEYRING_H
#define CEPH_KEYRING_H

#include "common/config.h"

#include "auth/Crypto.h"
#include "auth/Auth.h"

class KeyRing {
  map<EntityName, EntityAuth> keys;

  int set_modifier(const char *type, const char *val, EntityName& name, map<string, bufferlist>& caps);
  void decode_plaintext(bufferlist::iterator& bl);
public:
  map<EntityName, EntityAuth>& get_keys() { return keys; }  // yuck

  int load(const std::string &filename);
  void print(ostream& out);

  // accessors
  bool get_auth(const EntityName& name, EntityAuth &a) const {
    map<EntityName, EntityAuth>::const_iterator k = keys.find(name);
    if (k == keys.end())
      return false;
    a = k->second;
    return true;
  }
  bool get_secret(const EntityName& name, CryptoKey& secret) const {
    map<EntityName, EntityAuth>::const_iterator k = keys.find(name);
    if (k == keys.end())
      return false;
    secret = k->second.key;
    return true;
  }
  void get_master(CryptoKey& dest) const {
    get_secret(g_conf.name, dest);
  }

  // modifiers
  void add(const EntityName& name, EntityAuth &a) {
    keys[name] = a;
  }
  void set_caps(EntityName& name, map<string, bufferlist>& caps) {
    keys[name].caps = caps;
  }
  void set_uid(EntityName& ename, uint64_t auid) {
    keys[ename].auid = auid;
  }
  void set_key(EntityName& ename, CryptoKey& key) {
    keys[ename].key = key;
  }
  void import(KeyRing& other);

  // encoders
  void encode(bufferlist& bl) const {
    __u8 struct_v = 1;
    ::encode(struct_v, bl);
    ::encode(keys, bl);
  }
  void decode(bufferlist::iterator& bl);
};
WRITE_CLASS_ENCODER(KeyRing)

extern KeyRing g_keyring;

#endif
