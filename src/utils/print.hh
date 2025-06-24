#ifndef HILL__UTILS__PRINT_HH_INCLUDED
#define HILL__UTILS__PRINT_HH_INCLUDED

#include <algorithm>
#include <iomanip>
#include <string>
#include <stddef.h>
#include <stdint.h>

namespace hill::utils {

	/// <summary>
	/// Binary block to string
	/// </summary>
	/// <param name="p">Pointer to data</param>
	/// <param name="size">Size of data</param>
	/// <param name="col_cnt">Amount of columns to print</param>
	/// <param name="header">Header text, max 11 printable characters</param>
	inline std::string bintostr(
		const uint8_t *p,
		size_t size,
		size_t col_cnt = 16u,
		const char *header = "",
		bool show_ascii = true)
	{
		std::stringstream ss;

		ss << std::left
			<< std::setfill(' ')
			<< std::setw(11)
			<< header;

		ss << std::right
			<< std::hex
			<< std::uppercase
			<< std::setfill('0');
		for (size_t i=0; i<col_cnt; ++i) {
			ss << " " << std::setw(2) << i;
		}
		ss << '\n';

		for (size_t i=0; i<size; ++i) {
			if (i % col_cnt == 0) {
				ss << "0x"
					<< std::hex
					<< std::uppercase
					<< std::setw(8)
					<< i << ":";
			}
			ss << " "
				<< std::hex
				<< std::uppercase
				<< std::setfill('0')
				<< std::setw(2)
				<< +p[i];

			if ((i+1) % col_cnt == 0) {
				if (show_ascii) {
					ss << "  ";
					for (size_t ii=0; ii<col_cnt; ++ii) {
						uint8_t val = p[i + ii + 1 - col_cnt];
						if (std::isprint(val)) {
							ss << " " << std::nouppercase << val;
						} else {
							ss << " .";
						}
					}
				}
				ss << '\n';
			}
		}

		size_t remailing = col_cnt - (size % col_cnt);
		if (remailing != col_cnt) {
			if (show_ascii) {
				ss << std::setfill(' ') << std::setw(remailing * 3) << "";
				ss << "  ";
				for (size_t ii = 0; ii<remailing; ++ii) {
					uint8_t val = p[size + ii - std::min(col_cnt, size)];
					if (std::isprint(val)) {
						ss << " " << std::nouppercase << val;
					} else {
						ss << " .";
					}
				}
			}
			ss << '\n';
		}

		return ss.str();
	}
}

#endif // !HILL__UTILS__PRINT_HH_INCLUDED
