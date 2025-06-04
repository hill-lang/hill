#ifndef HILL__UTILS__JSON_WRITER_HPP_INCLUDED
#define HILL__UTILS__JSON_WRITER_HPP_INCLUDED

#include <filesystem>
#include <fstream>
#include <memory>
#include <ostream>
#include <stddef.h>
#include <stdint.h>
#include <vector>

namespace hill::utils {

	class json_writer_exception : public std::exception {
		const char *what() const noexcept override { return "JSON writer exception"; }
	};

	class json_writer_internal_exception : public std::exception {
		const char *what() const noexcept override { return "JSON writer internal exception"; }
	};

	struct json_writer {
		json_writer(const std::filesystem::path &fpath)
		{
			auto ofs = std::make_shared<std::ofstream>();
			ofs->open(fpath);
			if (!ofs || !ofs->is_open()) {
				throw json_writer_exception();
			}

			this->os = ofs;
		}

		json_writer(const std::shared_ptr<std::ostream> &os): os(os) {}

		~json_writer()
		{
			close();
		}

		void close()
		{
			if (this->os) {
				while (state.size()) {
					pop();
				}
				this->os->flush();
				os = nullptr;
			}
		}

		/// <summary>
		/// Start a standalone object. E.g. Inside an array or top level
		/// </summary>
		void obj()
		{
			if (!anonymous_allowed()) throw json_writer_exception();
			if (state.size() > 0 && state.back().element_count++ > 0) {
				*os << ',';
			}

			*os << '{';
			state.push_back(json_writer_state(json_node_kind::OBJECT));
		}

		/// <summary>
		/// Start an object inside an object
		/// </summary>
		/// <param name="key"></param>
		void obj(const std::string &key)
		{
			obj_header(key);
			*os << '{';
			state.push_back(json_writer_state(json_node_kind::OBJECT));
		}

		void obj_f32(const std::string &key, float value)
		{
			obj_header(key);
			*os << value;
		}

		void obj_i32(const std::string &key, int32_t value)
		{
			obj_header(key);
			*os << value;
		}

		void obj_str(const std::string &key, const std::string &value)
		{
			obj_header(key);
			*os << '"' << value << '"';
		}

		void obj_bool(const std::string &key, bool value)
		{
			obj_header(key);
			*os << (value ? "true" : "false");
		}

		void obj_raw(const std::string &key, const std::string &value)
		{
			obj_header(key);
			*os << value;
		}

		/// <summary>
		/// Start a standalone array. E.g. Inside an array or top level
		/// </summary>
		void arr()
		{
			if (!anonymous_allowed()) throw json_writer_exception();
			if (state.size() > 0 && state.back().element_count++ > 0) {
				*os << ',';
			}

			*os << '[';
			state.push_back(json_writer_state(json_node_kind::ARRAY));
		}

		/// <summary>
		/// Start an array inside an object
		/// </summary>
		void arr(const std::string &key)
		{
			obj_header(key);
			*os << '[';
			state.push_back(json_writer_state(json_node_kind::ARRAY));
		}

		void arr_f32(float value)
		{
			arr_header();
			*os << value;
		}

		void arr_i32(int32_t value)
		{
			arr_header();
			*os << value;
		}

		void arr_str(const std::string &value)
		{
			arr_header();
			*os << '"' << value << '"';
		}

		void arr_bool(bool value)
		{
			arr_header();
			*os << (value ? "true" : "false");
		}

		void arr_raw(const std::string &value)
		{
			arr_header();
			*os << value;
		}

		void pop()
		{
			if (state.size() == 0) throw json_writer_exception();

			switch (state.back().kind) {
			case json_node_kind::OBJECT: *os << '}'; break;
			case json_node_kind::ARRAY: *os << ']'; break;
			default: throw json_writer_internal_exception();
			}

			state.pop_back();
		}

	private:
		enum class json_node_kind {
			OBJECT,
			ARRAY,
		};

		struct json_writer_state {
			json_writer_state(json_node_kind kind): kind(kind), element_count(0) {}

			json_node_kind kind;
			size_t element_count;
		};

	private:
		void obj_header(const std::string &key)
		{
			if (state.size() == 0) obj();
			//if (state.size() == 0) throw json_writer_internal_exception();
			if (state.back().kind != json_node_kind::OBJECT) throw json_writer_exception();
			if (state.back().element_count++ > 0) *os << ',';
			*os << '"' << key << "\":";
		}

		void arr_header()
		{
			if (state.size() == 0) throw json_writer_internal_exception();
			if (state.back().kind != json_node_kind::ARRAY) throw json_writer_exception();
			if (state.back().element_count++ > 0) *os << ',';
		}

		/// <summary>
		/// Check if creating anonymous nodes is allowed
		/// </summary>
		bool anonymous_allowed()
		{
			if (state.size() == 0) {
				return true;
			}

			switch (state.back().kind) {
			case json_node_kind::ARRAY:
				return true;
			default:
				return false;
			}
		}

	private:
		std::shared_ptr<std::ostream> os;
		std::vector<json_writer_state> state;
	};
}

#endif /* HILL__UTILS__JSON_WRITER_HPP_INCLUDED */
