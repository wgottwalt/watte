/*
 *  Watte - weird and trivially tiny editor
 *  Copyright (C) 2020 Wilken 'Akiko' Gottwalt <akiko@linux-addicted.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <clocale>
#include <fstream>
#include <string>
#include <vector>
#include <ncurses.h>

// TODO: TAB key handling is really weird, so I ignore it for now
// TODO: shifted keys are not ignored (and all printabled function keys)

const std::string title = "Watte - weird and trivially tiny editor";
const std::string version = "0.9.0";

class Editor {
public:
    //--- public constructors ---
    Editor(const std::string &filename)
    : _data(), _filename(filename),
#if DEBUG
      _last_action(),
#endif
      _xpos(0), _ypos(1), _sline(0), _running(true)
    {
        std::setlocale(LC_ALL, "");
        ::initscr();
        ::start_color();
        ::keypad(stdscr, true);
        ::noecho();
        ::cbreak();
        ::raw();

        ::init_pair(1, COLOR_WHITE, COLOR_BLUE);
        ::init_pair(2, COLOR_WHITE, COLOR_BLUE);

        if (loadFile())
        {
#if DEBUG
            _last_action = "opened " + _filename;
#endif
        }
        else
        {
            _data.push_back("");
#if DEBUG
            _last_action = "started new file " + _filename;
#endif
        }

        drawGUI();
        ::refresh();
    }

    Editor(const Editor &rhs) = delete;
    Editor(Editor &&rhs) = delete;

    ~Editor()
    {
        ::endwin();
    }

    //--- public operators ---
    Editor &operator=(const Editor &rhs) = delete;
    Editor &operator=(Editor &&rhs) = delete;

    //--- public methods ---
    int32_t run()
    {
        while (_running)
        {
            drawGUI();
            ::refresh();
            processInput(::getch());
        }

        return 0;
    }

    void drawGUI()
    {
        const int32_t max_height = std::min(LINES - 2, static_cast<int32_t>(_data.size() - _sline));
        std::string header = title + " (" + version + ") '" + _filename + "'";
        std::string status = std::to_string(_data.size()) + " lines - "
                             + std::to_string(_xpos) + "," + std::to_string(_ypos - 1 + _sline);
        std::string footer = "(F1) reload file | (F2) save file | (F12) quit";
        std::string buffer;

        // header = title
        header.resize(COLS - status.size(), ' ');
        header += status;
        ::attron(COLOR_PAIR(1));
        mvaddnstr(0, 0, header.c_str(), header.size());
        ::attroff(COLOR_PAIR(1));

        // footer = status + buttons
#if DEBUG
        footer += " <> " + _last_action;
#endif
        footer.resize(COLS, ' ');
        ::attron(COLOR_PAIR(2) | A_BOLD);
        mvaddnstr(LINES - 1, 0, footer.c_str(), footer.size());
        ::attroff(COLOR_PAIR(2) | A_BOLD);

        // editor space
        for (int32_t i = 0; i < max_height; ++i)
        {
            buffer = _data[i + _sline];
            buffer.resize(COLS, ' ');
            mvaddnstr(i + 1, 0, buffer.c_str(), buffer.size());
        }

        ::move(_ypos, _xpos);
    }

    void processInput(const int32_t key)
    {
        const char chr = key;
        int32_t lines_below = std::max(0, static_cast<int32_t>(_data.size()) - _sline);
        int32_t max_height = std::min(LINES - 2, lines_below);
        int32_t max_width = std::min(COLS, static_cast<int32_t>(_data[_ypos + _sline - 1].size()));
        int32_t old_xpos = _xpos;
        int32_t old_ypos = _ypos;

        switch (key)
        {
            case KEY_F(1):
                loadFile();
#if DEBUG
                _last_action = "reloaded " + _filename;
#endif
                break;

            case KEY_F(2):
                saveFile();
#if DEBUG
                _last_action = "saved " + _filename;
#endif
                break;

            case KEY_F(12):
                _running = false;
                break;

            case KEY_UP:
                _ypos = std::max(_ypos - 1, 1);
                if ((old_ypos == _ypos) && (_sline > 0))
                    --_sline;
                max_width = std::min(COLS, static_cast<int32_t>(_data[_ypos + _sline - 1].size()));
                _xpos = std::min(_xpos, max_width);
                break;

            case KEY_DOWN:
                _ypos = std::min(_ypos + 1, max_height);
                if ((old_ypos == _ypos) && ((lines_below - LINES + 2) > 0))
                    ++_sline;
                max_width = std::min(COLS, static_cast<int32_t>(_data[_ypos + _sline - 1].size()));
                _xpos = std::min(_xpos, max_width);
                break;

            case KEY_LEFT:
                _xpos = std::max(_xpos - 1, 0);
                break;

            case KEY_RIGHT:
                _xpos = std::min(_xpos + 1, max_width);
                break;

            case KEY_DC: // delete char = delete
                if (_xpos < max_width)
                    _data[_ypos + _sline - 1].erase(_xpos, 1);
                else if ((_ypos > 1) || (_sline > 0)) // line wrapping delete
                {
                    auto &next_line = _data[_ypos + _sline];

                    _data[_ypos + _sline - 1] += next_line;
                    _data.erase(_data.begin() + _ypos + _sline);
                }
                break;

            case KEY_BACKSPACE:
                if (_xpos > 0)
                    _data[_ypos + _sline - 1].erase(_xpos-- - 1, 1);
                else if ((_ypos > 1) || (_sline > 0)) // line wrapping backspace
                {
                    auto &prev_line = _data[_ypos + _sline - 2];

                    old_xpos = prev_line.size();
                    prev_line += _data[_ypos + _sline - 1];
                    _data.erase(_data.begin() + _ypos + _sline - 1);

                    _ypos = std::max(_ypos - 1, 1);
                    if ((old_ypos == _ypos) && (_sline > 0))
                        --_sline;
                    max_width = std::min(COLS,
                                         static_cast<int32_t>(_data[_ypos + _sline - 1].size()));
                    _xpos = std::min(old_xpos, max_width);
                }
                break;

            case KEY_HOME:
                _xpos = 0;
                break;

            case KEY_END:
                _xpos = max_width;
                break;

            case KEY_PPAGE: // prev page = uses half page scrolling
                _ypos = std::max(_ypos - (LINES / 2), 1);
                if ((old_ypos == _ypos) && (_sline > 0))
                    _sline = std::max(_sline - (LINES / 2), 0);
                max_width = std::min(COLS, static_cast<int32_t>(_data[_ypos + _sline - 1].size()));
                _xpos = std::min(_xpos, max_width);
                break;

            case KEY_NPAGE:
                _ypos = std::min(_ypos + (LINES / 2), max_height);
                if ((old_ypos == _ypos) && ((lines_below + LINES + 2) > 0))
                    _sline = std::min(_sline + (LINES / 2), lines_below);
                max_width = std::min(COLS, static_cast<int32_t>(_data[_ypos + _sline - 1].size()));
                _xpos = std::min(_xpos, max_width);
                break;

            case KEY_ENTER:
            case 10:
                if (_xpos == max_width)
                    _data.insert(_data.begin() + _ypos + _sline, "");
                else
                {
                    const std::string substr = _data[_ypos + _sline - 1].substr(_xpos);

                    _data[_ypos + _sline - 1].erase(_xpos);
                    _data.insert(_data.begin() + _ypos + _sline, substr);
                }
                lines_below = std::max(0, static_cast<int32_t>(_data.size()) - _sline);
                max_height = std::min(LINES - 2, lines_below);
                _ypos = std::min(_ypos + 1, max_height);
                if ((old_ypos == _ypos) && ((lines_below - LINES + 2) > 0))
                    ++_sline;
                _xpos = 0;
                break;

            default:
                if (std::isprint(chr))
                    _data[_ypos - 1 + _sline].insert(_xpos++, 1, chr);
        }
    }

    bool loadFile()
    {
        if (std::ifstream ifile(_filename); ifile.is_open() && ifile.good())
        {
            std::string line;

            _data.clear();
            while (std::getline(ifile, line))
                _data.insert(_data.end(), line);
            ifile.close();

            return true;
        }

        return false;
    }

    bool saveFile()
    {
        if (std::ofstream ofile(_filename); ofile.is_open() && ofile.good())
        {
            for (auto &line : _data)
                ofile << line << '\n';
            ofile.close();

            return true;
        }

        return false;
    }

private:
    //--- private properties ---
    std::vector<std::string> _data;
    std::string _filename;
#if DEBUG
    std::string _last_action;
#endif
    int32_t _xpos;
    int32_t _ypos;
    int32_t _sline;
    bool _running;
};

int32_t main(int32_t argc, char **argv)
{
    Editor ed((argc == 2) ? argv[1] : "noname.txt");

    return ed.run();
}
