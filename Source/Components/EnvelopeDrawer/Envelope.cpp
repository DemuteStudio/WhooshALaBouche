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
