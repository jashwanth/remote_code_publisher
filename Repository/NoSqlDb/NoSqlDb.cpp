/////////////////////////////////////////////////////////////////////
// NoSqlDb.cpp - key/value pair in-memory database                 //
//                                                                 //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017       //
/////////////////////////////////////////////////////////////////////

#include "NoSqlDb.h"
#include <iostream>

using StrData = std::string;
using intData = int;
using Key = NoSqlDb<StrData>::Key;
using Keys = NoSqlDb<StrData>::Keys;

int main()
{
  std::cout << "\n  Demonstrating NoSql Helper Code";
  std::cout << "\n =================================\n";

  std::cout << "\n  Creating and saving NoSqlDb elements with string data";
  std::cout << "\n -------------------------------------------------------\n";

  NoSqlDb<StrData> db;

  Element<StrData> elem1;
  elem1.name = "elem1";
  elem1.category = "test";
  elem1.description = "elem1 description data";
  elem1.data = "elem1's StrData";
  elem1.children.clear();
  db.save(elem1.name, elem1);

  Element<StrData> elem2;
  elem2.name = "elem2";
  elem2.category = "test";
  elem2.description = "elem2 description data";
  elem2.data = "elem2's StrData";
  elem2.children.clear();


  db.save(elem2.name, elem2);
  db.deleteKey(elem2);
  Element<StrData> elem3;
  elem3.name = "elem3";
  elem3.category = "test";
  elem3.data = "elem3's StrData";
  elem3.description = "elem3 description data";
  elem3.children.clear();

  db.save(elem3.name, elem3);

  std::cout << "\n  Retrieving elements from NoSqlDb<string>";
  std::cout << "\n ------------------------------------------\n";

  std::cout << "\n  size of db = " << db.count() << "\n";
  Keys keys = db.keys();
  for (Key key : keys)
  {
    std::cout << "\n  " << key << ":";
    std::cout << db.value(key).show();
  }
  std::cout << "\n\n";
  db.deleteKey(elem3);
  std::cout << "\n  Deleted element3 from NoSqlDb ";
  std::cout << "\n --------------------------------\n";

  keys = db.keys();
  for (Key key : keys)
  {
	  std::cout << "\n  " << key << ":";
	  std::cout << db.value(key).show();
  }
  std::cout << "\n\n";

  std::cout << "\n  Creating and saving NoSqlDb elements with int data";
  std::cout << "\n ----------------------------------------------------\n";

 /* NoSqlDb<intData> idb;

  Element<intData> ielem1;
  ielem1.name = "elem1";
  ielem1.category = "test";
  ielem1.data = 1;

  idb.save(ielem1.name, ielem1);

  Element<intData> ielem2;
  ielem2.name = "elem2";
  ielem2.category = "test";
  ielem2.data = 2;

  idb.save(ielem2.name, ielem2);

  Element<intData> ielem3;
  ielem3.name = "elem3";
  ielem3.category = "test";
  ielem3.data = 3;

  idb.save(ielem3.name, ielem3);

  Element<intData> ielem4;
  ielem3.name = "elem4";
  ielem3.category = "test";
  ielem3.data = 4;

  idb.save(ielem3.name, ielem3);

  std::cout << "\n  Retrieving elements from NoSqlDb<int>";
  std::cout << "\n ---------------------------------------\n";

  std::cout << "\n  size of idb = " << idb.count() << "\n";
  Keys ikeys = idb.keys();
  for (Key key : ikeys)
  {
    std::cout << "\n  " << key << ":";
    std::cout << idb.value(key).show();
  }*/
  std::cout << "\n\n";
}