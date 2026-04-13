#ifndef TOKENIZER_H
#define TOKENIZER_H

//#include "ft_irc.hpp"
#include <string>
#include <vector>

enum State {Param, Space, LongParam};

class Tokenizer
{
	private:
		std::string content; // raw input line to tokenize
		std::vector<std::string> tokens; // output tokens
		State state; // current fms state
		unsigned int current_position; // index for the current character being processed

		std::string buffer; // for the current token being processed

		void tokenize_param(); // when state == Param: read characters into buffer until a delimiter (likely space or end of input), then push_token() and transition state.
		void tokenize_space(); //when state == Space: skip spaces; if a non-space appears:
		void tokenize_long_param(); // when state == LongParam: read characters into buffer until a delimiter (likely space or end of input), then push_token() and transition state.
		char current_char() const; // returns the current character being processed
		void accumulate(); // adds the current character to the buffer
		void push_token(); // if buffer is non-empty, tokens.push_back(buffer) and buffer.clear()
		void move(); // advances current_position++

	public:
		Tokenizer(std::string content); // stores content, initializes state, sets current_position = 0, clears containers
		virtual ~Tokenizer();
		void tokenize(); // runs loop over content
		void change_state(State new_state); // updates state
		std::vector<std::string> get_tokens();
};

#endif