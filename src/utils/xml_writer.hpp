#ifndef HILL__UTILS__XML_WRITER_HPP_INCLUDED
#define HILL__UTILS__XML_WRITER_HPP_INCLUDED

#include "../utils/string.hpp"

#include <filesystem>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <memory>

namespace hill::utils {

	class xml_writer_exception : std::exception {
		const char *what() const noexcept override { return "XML writer exception"; }
	};

	class xml_writer_internal_exception : std::exception {
		const char *what() const noexcept override { return "XML writer internal exception"; }
	};

	enum class xml_version {
		V1_0,
	};

	enum class xml_encoding {
		UTF_8,
	};

	constexpr const char *xml_version_str(xml_version v)
	{
		switch (v) {
		case xml_version::V1_0: return "1.0";
		default: throw xml_writer_internal_exception();
		}
	}

	constexpr const char *xml_encoding_str(xml_encoding v)
	{
		switch (v) {
		case xml_encoding::UTF_8: return "UTF-8";
		default: throw xml_writer_internal_exception();
		}
	}

	struct xml_writer {
		xml_writer(const std::filesystem::path &fpath)
			: version(xml_version::V1_0), encoding(xml_encoding::UTF_8)
		{
			auto ofs = std::make_shared<std::ofstream>();
			ofs->open(fpath);
			if (!ofs || !ofs->is_open()) {
				throw xml_writer_exception();
			}

			os = ofs;

			write_header();
		}

		xml_writer(const std::shared_ptr<std::ostream> &os)
			: os(os), version(xml_version::V1_0), encoding(xml_encoding::UTF_8)
		{
			write_header();
		}

		~xml_writer()
		{
			close();
		}

		std::shared_ptr<std::ostream> os;
		xml_version version;
		xml_encoding encoding;

		void close()
		{
			if (os) {
				flush();
				os = nullptr;
			}
		}

		void push_section(const std::string &sname)
		{
			if (section_stack.size() > 0) {
				auto &s = section_stack.back();
				if (!s.flushed_start) {
					write_section_start(s);
					s.flushed_start = true;
				}
			}

			section_stack.push_back(xml_section(sname));
		}

		void pop_section()
		{
			if (section_stack.size()==0) {
				return;
			}

			auto &s = section_stack.back();
			flush_section(s);
			section_stack.pop_back();
		}

		void set_attribute(const std::string &key, const std::string &value)
		{
			if (section_stack.size()==0) {
				return;
			}

			auto &s = section_stack.back();
			s.attribs[key] = value;
		}

		void set_self_closing(bool self_closing)
		{
			if (section_stack.size()==0) {
				return;
			}

			auto &s = section_stack.back();
			s.self_closing = self_closing;
		}

		void add_text(const std::string &text)
		{
			if (section_stack.size()==0) {
				return;
			}

			auto &s = section_stack.back();
			if (!s.text.empty()) {
				s.text += ' ';
			}

			std::string t = text;
			utils::trim(t);
			s.text += t;
		}

	private:
		struct xml_section {
			explicit xml_section(const std::string &name)
				: name(name), self_closing(false), flushed_start(false) {}

			std::string name;
			std::unordered_map<std::string, std::string> attribs;
			std::string text;
			bool self_closing;

			bool flushed_start;

			void write_indent(std::ostream &os, size_t indent_level) const
			{
				for (size_t i=0; i<indent_level; ++i) {
					//os << "    ";
					os << "\t";
				}
			}

			void write_start(std::ostream &os, size_t indent_level) const
			{
				write_indent(os, indent_level);
				os << '<' << this->name;
				for (const auto &attr : this->attribs) {
					os << ' ' << attr.first
						<< "=\"" << attr.second << '"';
				}
				if (this->self_closing) {
					os << " /";
				}
				os << ">\n";
			}

			void write_text(std::ostream &os, size_t indent_level) const
			{
				write_indent(os, indent_level);
				for (const auto &c : this->text) {
					os << c;
					if (c == '\n') {
						write_indent(os, indent_level);
					}
				}
				os << '\n';
			}

			void write_end(std::ostream &os, size_t indent_level) const
			{
				write_indent(os, indent_level);
				os << "</" << this->name << ">\n";
			}
		};

		std::vector<xml_section> section_stack;

		void flush_section(const xml_section &s)
		{
			if (!s.flushed_start) {
				write_section_start(s);
			}
			if (!s.text.empty()) {
				write_section_text(s);
			}
			if (!s.self_closing) {
				write_section_end(s);
			}
		}

		void flush()
		{
			while (section_stack.size() > 0) {
				auto &s = section_stack.back();
				flush_section(s);
				section_stack.pop_back();
			}
			os->flush();
		}

		void write_header()
		{
			*os << "<?xml"
				<< " version=\"" << xml_version_str(this->version) << '"'
				<< " encoding=\"" << xml_encoding_str(this->encoding) << '"'
				<< "?>\n";
		}

		void write_section_start(const xml_section &s)
		{
			s.write_start(*os, this->section_stack.size()-1);
		}

		void write_section_text(const xml_section &s)
		{
			s.write_text(*os, this->section_stack.size());
		}

		void write_section_end(const xml_section &s)
		{
			s.write_end(*os, this->section_stack.size()-1);
		}
	};
}

#endif /* HILL__UTILS__XML_WRITER_HPP_INCLUDED */
