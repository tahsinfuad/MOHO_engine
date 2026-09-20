#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <cctype>
#include <algorithm>

namespace cfg
{
    class Settings
    {
    public:
        void load(const std::string& path)
        {
            std::ifstream file(path);

            if (!file.is_open())
            {
                throw std::runtime_error("Failed to open settings file: " + path);
            }

            values_.clear();
            order_.clear();

            std::string line;
            std::string section;

            while (std::getline(file, line))
            {
                line = trim(line);

                if (line.empty() || line[0] == '#')
                    continue;

                strip_comment(line);

                if (line.empty())
                    continue;

                if (line.front() == '[' && line.back() == ']')
                {
                    section = trim(line.substr(1, line.size() - 2));
                    continue;
                }

                size_t separator = line.find('=');
                if (separator == std::string::npos)
                    continue;

                std::string key   = trim(line.substr(0, separator));
                std::string value = trim(line.substr(separator + 1));

                unquote(value);

                if (!section.empty())
                    key = section + "." + key;

                set_raw(key, value);
            }
        }

        void save(const std::string& path) const
        {
            std::ofstream file(path);

            if (!file.is_open())
            {
                throw std::runtime_error("Failed to open settings file for writing: " + path);
            }

            std::string current_section;

            for (const auto& key : order_)
            {
                std::string section, name;
                split_key(key, section, name);

                if (section != current_section)
                {
                    if (!section.empty())
                        file << "\n[" << section << "]\n";

                    current_section = section;
                }

                file << name << " = " << quote_if_needed(values_.at(key)) << "\n";
            }
        }

        bool has(const std::string& key) const
        {
            return values_.find(key) != values_.end();
        }

        // Usage: int width = settings.get<int>("window.width");
        template <typename T>
        T get(const std::string& key) const
        {
            return convert<T>(values_.at(key));
        }

        // Usage: int width = settings.get<int>("window.width", 1280);
        template <typename T>
        T get(const std::string& key, const T& fallback) const
        {
            auto it = values_.find(key);
            if (it == values_.end())
                return fallback;

            return convert<T>(it->second);
        }

        void set(const std::string& key, const std::string& value)
        {
            set_raw(key, value);
        }

    private:
        std::unordered_map<std::string, std::string> values_;
        std::vector<std::string> order_;   // preserves insertion order for save()

        void set_raw(const std::string& key, const std::string& value)
        {
            if (values_.find(key) == values_.end())
                order_.push_back(key);

            values_[key] = value;
        }

        static void split_key(const std::string& key, std::string& section, std::string& name)
        {
            size_t dot = key.rfind('.');

            if (dot == std::string::npos)
            {
                section.clear();
                name = key;
            }
            else
            {
                section = key.substr(0, dot);
                name    = key.substr(dot + 1);
            }
        }

        static std::string trim(const std::string& str)
        {
            size_t start = 0;
            size_t end = str.size();

            while (start < end && std::isspace(static_cast<unsigned char>(str[start])))
                ++start;

            while (end > start && std::isspace(static_cast<unsigned char>(str[end - 1])))
                --end;

            return str.substr(start, end - start);
        }

        // Strips a trailing comment, but ignores '#' that appears inside quotes.
        static void strip_comment(std::string& line)
        {
            bool in_single = false;
            bool in_double = false;

            for (size_t i = 0; i < line.size(); ++i)
            {
                char c = line[i];

                if (c == '\'' && !in_double)
                {
                    in_single = !in_single;
                }
                else if (c == '"' && !in_single)
                {
                    in_double = !in_double;
                }
                else if (c == '#' && !in_single && !in_double)
                {
                    line = trim(line.substr(0, i));
                    return;
                }
            }
        }

        static void unquote(std::string& value)
        {
            if (value.size() >= 2 &&
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\'')))
            {
                value = value.substr(1, value.size() - 2);
            }
        }

        static std::string quote_if_needed(const std::string& value)
        {
            bool needs_quotes = value.empty() ||
                value.find(' ') != std::string::npos ||
                value.find('#') != std::string::npos;

            if (needs_quotes)
                return "\"" + value + "\"";

            return value;
        }

        template <typename T>
        static T convert(const std::string& raw);
    };

    // ---- type conversions (add more specializations here if you need them) ----

    template <>
    inline std::string Settings::convert<std::string>(const std::string& raw)
    {
        return raw;
    }

    template <>
    inline int Settings::convert<int>(const std::string& raw)
    {
        return std::stoi(raw);
    }

    template <>
    inline float Settings::convert<float>(const std::string& raw)
    {
        return std::stof(raw);
    }

    template <>
    inline double Settings::convert<double>(const std::string& raw)
    {
        return std::stod(raw);
    }

    template <>
    inline bool Settings::convert<bool>(const std::string& raw)
    {
        std::string lower = raw;
        std::transform(lower.begin(), lower.end(), lower.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

        return lower == "true" || lower == "1" || lower == "yes" || lower == "on";
    }
}