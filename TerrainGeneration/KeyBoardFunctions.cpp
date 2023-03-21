
std::map<char, bool> KeyBoardFunctions::keys;
std::map<char, bool> KeyBoardFunctions::specialKeys;

KeyBoardFunctions::KeyBoardFunctions()
{
}

KeyBoardFunctions::KeyBoardFunctions(glm::vec3 pos)
{
	position = pos;
}

KeyBoardFunctions::~KeyBoardFunctions()
{
}