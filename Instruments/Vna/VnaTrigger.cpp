

// RsaToolbox includes
#include "General.h"
#include "VnaTrigger.h"
#include "VnaChannel.h"
#include "Vna.h"
#include "VnaScpi.h"
using namespace RsaToolbox;

// Qt includes
#include <QString>


VnaTrigger::VnaTrigger(QObject *parent) :
    QObject(parent)
{
    placeholder.reset(new Vna());
    _vna = placeholder.data();
    _channel.reset(new VnaChannel());
    _channelIndex = 0;
}

VnaTrigger::VnaTrigger(const VnaTrigger &other)
{
    if (other.isFullyInitialized()) {
        _vna = other._vna;
        _channel.reset(new VnaChannel(*other._channel.data()));
        _channelIndex = other._channelIndex;
    }
    else {
        placeholder.reset(new Vna());
        _vna = placeholder.data();
        _channel.reset(new VnaChannel());
        _channelIndex = 0;
    }
}
VnaTrigger::VnaTrigger(Vna *vna, VnaChannel *channel, QObject *parent) :
    QObject(parent)
{
    _vna = vna;
    _channel.reset(new VnaChannel(*channel));
    _channelIndex = channel->index();
}

VnaTrigger::VnaTrigger(Vna *vna, uint channelIndex, QObject *parent) :
    QObject(parent)
{
    _vna = vna;
    _channel.reset(new VnaChannel(vna, channelIndex, this));
    _channelIndex = channelIndex;
}
VnaTrigger::~VnaTrigger() {

}

VnaTrigger::Source VnaTrigger::source() {
    QString scpi = ":TRIG%1:SOUR?\n";
    return VnaScpi::toTriggerSource(_vna->query(scpi));
}
void VnaTrigger::setSource(Source source) {
    QString scpi = ":TRIG:SOUR %1\n";
    scpi = scpi.arg(VnaScpi::toString(source));
    _vna->write(scpi);
}

void VnaTrigger::sendManualTrigger() {
    QString scpi = "*TRG\n";
    _vna->write(scpi);
}

uint VnaTrigger::delay_ms() {
    QString scpi = ":TRIG%1HOLD?\n";
    scpi = scpi.arg(_channelIndex);
    const double time_s = _vna->query(scpi).trimmed().toDouble();
    return uint(1000 * time_s);
}
void VnaTrigger::setDelay(uint time_ms) {
    QString scpi = ":TRIG%1HOLD %2 ms\n";
    scpi = scpi.arg(_channelIndex);
    scpi = scpi.arg(time_ms);
    _vna->write(scpi);
}

// Fill in

void VnaTrigger::operator=(VnaTrigger const &other) {
    if (other.isFullyInitialized()) {
        _vna = other._vna;
        _channel.reset(new VnaChannel(other._channel.data()));
        _channelIndex = other._channelIndex;
    }
    else {
        placeholder.reset(new Vna());
        _vna = placeholder.data();
        _channel.reset(new VnaChannel());
        _channelIndex = 0;
    }
}

// Private
bool VnaTrigger::isFullyInitialized() const {
    if (_vna == NULL)
        return(false);
    if (_vna == placeholder.data())
        return(false);

    //else
    return(true);
}