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
	return list_[index];
}

void envelope::add(node new_node)
{
	list_.emplace_back(new_node);
}
