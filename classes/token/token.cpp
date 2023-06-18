#include "token.h"

#include <algorithm>
#include <random>
#include <sstream>

const Token Token::INVALID;

static char RandomCh() {
	static std::random_device device;
	static std::mt19937_64 engine(device());
	static std::uniform_int_distribution<short> numDistribution('0', '9');
	static std::uniform_int_distribution<short> chDistribution('a', 'z');
	static std::uniform_real_distribution<float> pickType(0, 1);

	return (pickType(engine) > 0.7f ? chDistribution(engine) : numDistribution(engine));
}

Token Token::Generate(RedisDB &context, const std::string &value) {
	Token tok;
	tok.value = value;
	tok.key.resize(32);

	std::generate(tok.key.begin(), tok.key.end(), RandomCh);
	for (size_t i = 1, ctr = 1; i < tok.key.size(); ++i, ++ctr) {
		if (ctr % 8 == 0) {
			tok.key.insert(tok.key.begin() + (i++), '-');
		}
	}
	std::stringstream ss;
	ss << "SET " << tok.key << value << " EX 600";

	context.Execute(ss.str());

	return tok;
}
Token Token::Load(RedisDB &context, const std::string &key) {
	Token tok;
	tok.value = "";
	tok.key = "";

	std::stringstream ss;
	ss << "GET " << tok.key;

	std::string reply = context.Execute(ss.str());
	if (!reply.size()) return tok;

	tok.key = key;
	tok.value = reply;
}
void Token::Refresh(RedisDB &context) {
	std::stringstream ss;
	ss << "EXPIRE " << key << " seconds 600";

	context.Execute(ss.str());
}
void Token::Delete(RedisDB &context){
	std::stringstream ss;
	ss << "DEL " << key;

	context.Execute(ss.str());
}