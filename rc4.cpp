// source: https://gist.github.com/Mjiig/2727751
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <cstring>

class State
{
	unsigned char s[256];
	int i, j;

	void swap(int a, int b);

	public:
	unsigned char getbyte(void);
	State(unsigned char key[], int length );
};

State::State(unsigned char key[], int length)
{
	for(int k=0; k<256; k++)
	{
		s[k]=k;
	}

	j=0;
		
	for(i=0; i<256 ; i++)
	{
		j=(j + s[i] + key[i % length]) % 256;
		swap(i, j);
	}

	i=j=0;
}

void State::swap(int a, int b)
{
	unsigned char temp= s[i];
	s[i]=s[j];
	s[j]=temp;
}

unsigned char State::getbyte(void)
{
	i=(i+1)%256;
	j=(j+s[i])%256;
	swap(i, j);
	int index=(s[i]+s[j])%256;
	return s[index];
}


void parseargs(int argc, char ** argv, std::string & key, std::string & file, bool & hex)
{
	bool readkey = false ;
	bool readfile = false;
	bool toomanyargs =false;

	for( int i=1 ; i<argc ; i++ )
	{
		std::string arg = argv[i];
		if(arg=="-h")
		{
			hex=true;
		}
		else if(!readkey)
		{
			key=arg;
			readkey=true;
		}
		else if(!readfile)
		{
			file=arg;
			readfile=true;
		}
		else
		{
			toomanyargs=true;
		}
	}

	if(toomanyargs || !readfile || !readkey)
	{
		std::cout << "Usage is: " << argv[0] << " [-h] key file" << std::endl;
		exit(EXIT_FAILURE);
	}

	return;
}

void gethexdigit(char in, unsigned char & out)
{
	if(in>='0' && in<='9')
	{
		out += in-'0';
	}
	else if(in>='a' && in<='f')
	{
		out += in -'a' + 10;
	}
	else
	{
		std::cout << "Hex key contains letter outside range 0-9 or a-z: " << in << std::endl; 
		exit(EXIT_FAILURE);
	}
}



int gethexkey(unsigned char data[], std::string key)
{
	if(key.length() % 2) //key must be of even length if it's hex
	{
		std::cout << "Hex key must have an even number of characters" << std::endl;
		exit(EXIT_FAILURE);
	}

	if(key.length() > 512)
	{
		std::cout << "Hex key cannot be longer than 512 characters long" << std::endl;
		exit(EXIT_FAILURE);
	}

	unsigned char byte;
	size_t i;

	for(i=0; i < key.length(); i++)
	{
		gethexdigit(key[i], byte);
		byte <<= 4;
		i++;
		gethexdigit(key[i], byte);
		data[(i-1)/2]=byte;
	}
	return i/2;
}

int gettextkey(unsigned char data[], std::string key)
{
	if(key.length() > 256)
	{
		std::cout << "ASCII key must be 256 characters or less" <<std::endl;
		exit(EXIT_FAILURE);
	}

	size_t i;

	for(i=0; i<key.length(); i++)
	{
		data[i]=key[i];
	}

	return i;
}

int main(int argc, char **argv)
{
	std::string key, file;
	bool hex = false;
	parseargs(argc, argv, key, file, hex);

	int len=0;
	unsigned char keydata[256];

	if(hex)
		len=gethexkey(keydata, key);
	else
		len=gettextkey(keydata, key);

	State bytestream (keydata, len);

	std::fstream infile;
	infile.open(file.c_str(), std::ios::in | std::ios::binary);
	if(!infile.is_open())
	{
		std::cout << file << " does not exist" << std::endl;
		exit(EXIT_FAILURE);
	}

	if(file.find(".rc4", file.length()-4) != std::string::npos) //ie, if file ends with ".rc4"
	{
		file.erase(file.length()-4);
	}
	else
	{
		file.append(".rc4");
	}

	std::fstream outfile;
	outfile.open(file.c_str(), std::ios::in);
	if(outfile.is_open()) //file we are going to write to exists!
	{
		std::cout << file << " already exists, aborting to preserve it" << std::endl;
		exit(EXIT_FAILURE);
	}

	outfile.close();
	outfile.open(file.c_str(), std::ios::out | std::ios::binary);
	if(outfile.is_open()) //Test if we were able to open the file for writing
	{
		char inbyte;
		char outbyte;
		unsigned char streambyte;

		infile.get(inbyte);

		while(!infile.eof())
		{
			streambyte=bytestream.getbyte();
			outbyte=inbyte ^ streambyte;
			outfile.put(outbyte);
			infile.get(inbyte);
		}
	}
	else
	{
		std::cout << "Could not open " << file << " for writing\n" <<std::endl;
		exit(EXIT_FAILURE);
	}

	outfile.close();
	infile.close();

	std::cout << "Encryption finished, output to " << file << std::endl;

	return 0;
}
