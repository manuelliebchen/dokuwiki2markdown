#include <iostream>
#include <string.h>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <regex>

template <unsigned N>
struct pillar
{
    std::stringstream& in;
    char               data[N];
    char*              head;
    pillar(std::stringstream& in) : in(in) { head = data; }

    char operator++()
    {
        ++head;
        if (head == data + N)
        {
            head = data;
        }
        in.get(*head);
        return *head;
    }

    char operator[](int n)
    {
        n %= N;
        if (head + n < data)
        {
            return *(head + N + n);
        }
        return *(head + n);
    }
};

bool format_links(std::stringstream& in, std::stringstream& out)
{
    pillar<3> pill(in);
    ++pill;
    do
    {
        if (pill[0] == '[' && pill[-1] == '[')
        {
            ++pill;
            ++pill;
            std::string link;
            do
            {
                link += pill[-1];
                ++pill;
                if (in.eof())
                {
                    std::cerr << "Error while reading link\n";
                    return false;
                }
            } while (!(pill[-1] == '|' || (pill[0] == ']' && pill[-1] == ']')));

            if (pill[0] == ']' && pill[-1] == ']')
            {
                out << '[' << link << "](" << link << ")";
            }
            else
            {
                std::string name;
                ++pill;
                do
                {
                    name += pill[-1];
                    ++pill;
                    if (in.eof())
                    {
                        std::cerr << "Error while reading link name\n";
                        return false;
                    }
                } while (!(pill[0] == ']' && pill[-1] == ']'));
                out << '[' << name << "](" << link << ")";
            }
            ++pill;
            ++pill;
        }
        else
        {
            out << pill[-1];
            ++pill;
        }
    } while (!in.eof());
    return true;
};

bool format_headlines(std::stringstream& in, std::stringstream& out)
{
    char c, lastc;
    lastc = '\n';
    in.get(c);
    do
    {
        if (c == '=' && lastc == '\n')
        {
            int count = 0;
            do
            {
                ++count;
                in.get(c);
                if (in.eof())
                {
                    std::cerr << "Error while reading headline prefix\n";
                    return false;
                }
            } while (c == '=');
            for (int i = 0; i < 7 - count; ++i)
            {
                out << "#";
            }

            do
            {
                out << c;
                in.get(c);
                if (in.eof())
                {
                    break;
                }
            } while (!(c == '=' || c == '\n'));
            if (c != '\n')
            {
                do
                {
                    in.get(c);
                    if (in.eof())
                    {
                        break;
                    }
                } while (c == '=');
            }
        }
        else
        {
            out << c;
            lastc = c;
            in.get(c);
        }
    } while (!in.eof());
    return true;
};

bool format_tables(std::stringstream& in, std::stringstream& out)
{
    char c, lastc;
    lastc = '\n';
    in.get(c);
    do
    {
        if (c == '^')
        {
            out << "\n";
            int cols = 0;
            do
            {
                if (c == '^')
                {
                    out << '|';
                    cols++;
                }
                else
                {
                    out << c;
                }
                in.get(c);
                if (in.eof())
                {
                    std::cerr << "Error while reading table header\n";
                    return false;
                }
            } while (c != '\n');
            out << "\n";
            for (int i = 0; i < cols - 1; ++i)
            {
                out << "|---";
            }
            out << "|\n";

            int  count   = 0;
            bool in_link = false;
            do
            {
                if (c == '[' && lastc == '[')
                {
                    in_link = true;
                }
                if (c == ']' && lastc == ']')
                {
                    in_link = false;
                }
                if (c != '\n')
                {
                    if (c == '|' && !in_link)
                    {
                        count++;
                    }
                    out << c;
                    if (count == cols)
                    {
                        out << "\n";
                        count = 0;
                    }
                }

                lastc = c;
                in.get(c);
                if (in.eof())
                {
                    out << c;
                    break;
                }
            } while (
                !((c == '\n' && lastc == '\n') || (c == '^' && lastc == '\n')));
        }
        else
        {
            out << c;
            lastc = c;
            in.get(c);
        }
    } while (!in.eof());
    return true;
};

int main(int argc, char** argv)
{
    std::string file_name(argv[1]);
    std::regex  word_regex("(.*)(.txt)");
    if (!std::regex_match(file_name, word_regex))
    {
        return -1;
    }
    std::ifstream in(argv[1]);
    if (!in.is_open())
    {
        std::cerr << "Can not open file: " << argv[1] << std::endl;
        return -1;
    }

    std::stringstream in_stream;
    in_stream << in.rdbuf();
    in.close();
    std::stringstream headlineformater;
    if (!format_headlines(in_stream, headlineformater))
    {
        std::cerr << "Error while formating headlines\n";
        std::cerr << "Can not formate file: " << argv[1] << std::endl;
        in.close();
        return -1;
    }
    std::stringstream linkfree;
    if (!format_links(headlineformater, linkfree))
    {
        std::cerr << "Error while formating links\n";
        std::cerr << "Can not formate file: " << argv[1] << std::endl;
        in.close();
        return -1;
    }
    std::stringstream out;
    if (!format_tables(linkfree, out))
    {
        std::cerr << "Error while formating tables\n";
        std::cerr << "Can not formate file: " << argv[1] << std::endl;
        in.close();
        return -1;
    }

    std::ofstream file_out(argv[1]);
    file_out << out.rdbuf();
    file_out.close();
}
