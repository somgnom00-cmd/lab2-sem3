#pragma once
#include <string>
#include <ostream>
struct Database;

// Добавил обработку новых команд и сохранение новых типов структур.
bool executeLab(Database& database, const std::string& query, bool& handled);
void labHelp();
bool labLoadLine(Database& database, const std::string& line);
void labSave(Database& database, std::ostream& output);
void clearLabDatabase(Database& database);
