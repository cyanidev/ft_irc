#include "Tokenizer.hpp"

/// ---------------------------------
// CONVIERTE UN MENSAJE DE IRC RAW EN UN VECTOR DE STRINGS (TOKENS)
// POR EJEMPLO
// "PRIVMSG #channel :Holi"
// se vuelve:
// ["PRIVMSG", "#channel", "Holi"]
// ----------------------------------

//content: the raw string to tokenize
//state: keeps track of the current parsing state (Param, Space, LongParam)
//current_position: index of the character currently being read
//the constructor initializes the tokenizer with the raw string, 
//sets the state to Param (default), and starts at position 0
Tokenizer::Tokenizer(std::string raw_content) : content(raw_content),
	state(Param), current_position(0) {}

// main loop calls the right tokenizer function based on the current state
//<= content.size() ensures it handles the "end of string" as a token boundary
void Tokenizer::tokenize()
{
	while (current_position <= content.size())
	{
		if (state == Param)
		{
			tokenize_param();
		}
		else if (state == Space)
		{
			tokenize_space();
		}
		else if (state == LongParam)
		{
			tokenize_long_param();
		}
	}
}


//param is the default state for tokenizing regular parameters
// if it's a space, we switch to the Space state
//if buffer is not empty, we push the token
//if \0, we push whats in the buffer otherwise we just move
void Tokenizer::tokenize_param()
{
	switch (current_char())
	{
		case ' ':
			change_state(Space);
			if (!buffer.empty())
			{
				push_token();
			}
			move();
			break;
		case '\0':
			if (!buffer.empty())
			{
				push_token();
			}
			move();
			break;
		default:
			accumulate();
	}
}

//for skipping consecutive spaces ":" triggers LongParam state (IRC-style “rest of the line” parameter)
//and any other non-space character switches back to Param state
void Tokenizer::tokenize_space()
{
	switch (current_char())
	{
		case ' ':
			move();
			break;
		case '\0':
			move();
			break;
		case ':':
			change_state(LongParam);
			move();
			break;
		default:
			change_state(Param);
	}
}

//reads everything until the end of the line
void Tokenizer::tokenize_long_param()
{
	switch (current_char())
	{
		case '\0':
			push_token();
			move();
			break;
		default:
			accumulate();
	}
}

//returns the current character being read or \0 if at the end of the string
char Tokenizer::current_char() const
{
	if (current_position < content.size())
	{
		return (content[current_position]);
	}
	return ('\0');
}

//adds the char to buffer and moves to the next character
void Tokenizer::accumulate()
{
	buffer.push_back(current_char());
	move();
}


//increments current pos
void Tokenizer::move()
{
	current_position++;
}

//selfexplanatory
void Tokenizer::change_state(State new_state)
{
	state = new_state;
}

//saves buffer to token and clears it
void Tokenizer::push_token()
{
	tokens.push_back(buffer);
	buffer.clear();
}

std::vector<std::string> Tokenizer::get_tokens()
{
	return (tokens);
}

Tokenizer::~Tokenizer() {}