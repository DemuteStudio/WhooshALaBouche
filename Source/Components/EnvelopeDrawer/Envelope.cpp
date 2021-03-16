#include "Envelope.h"

envelope::envelope()
{
}

envelope::envelope(envelope* envelope)
{
	list_ = envelope->list_;
}

envelope::~envelope()
{
}

int envelope::get_size()
{
	return list_.size();
}

envelope::node envelope::operator[](const int index)
{
	auto iterator = list_.begin();
	for (int i = 0; i < index; ++i)
	{
		++iterator;
	}
	return *iterator;
}

void envelope::add(node new_node)
{
	list_.emplace_back(new_node);
}
