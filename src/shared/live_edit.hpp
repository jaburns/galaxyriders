#pragma once
#include <cstdint>
#include <unordered_map>
#include <string>
#include "lang_utils.hpp"
#include "network.hpp"

class LiveEdit : public NoCopy
{
public:
	struct Value
	{
		float min = 0.0f;
		float max = 1.0f;
		float value = 1.0f;
	};

	using ValueMap = std::unordered_map<std::string,Value>;

private:
    TCPServer m_server;
	std::unordered_map<std::string,Value> m_values;

public:
    static const uint16_t PORT = 1337;

    LiveEdit();
    ~LiveEdit();

    void update();

	static float get_value(const std::string& label, float min, float max);
};