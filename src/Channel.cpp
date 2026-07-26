#include "../includes/Channel.hpp"

Channel::Channel(const std::string &name)
    : _name(name),
      _topic(""),
      _key(""),
      _userLimit(0),
      _inviteOnly(false),
      _topicRestricted(false)
{}

Channel::Channel(const Channel &other)
{
    *this = other;
}

Channel &Channel::operator=(const Channel &other)
{
    if (this != &other)
    {
        _name = other._name;
        _topic = other._topic;
        _key = other._key;
        _userLimit = other._userLimit;
        _inviteOnly = other._inviteOnly;
        _topicRestricted = other._topicRestricted;

        _members = other._members;
        _operators = other._operators;
        _invited = other._invited;
    }
    return (*this);
}

Channel::~Channel()
{}

const std::string &Channel::getName() const
{
    return (_name);
}

const std::string &Channel::getTopic() const
{
    return (_topic);
}

bool Channel::isInviteOnly() const
{
    return (_inviteOnly);
}

bool Channel::isTopicRestricted() const
{
    return (_topicRestricted);
}

size_t Channel::getUserLimit() const
{
    return (_userLimit);
}

void Channel::setTopic(const std::string &topic)
{
    _topic = topic;
}

void Channel::setInviteOnly(bool value)
{
    _inviteOnly = value;
}

void Channel::setTopicRestricted(bool value)
{
    _topicRestricted = value;
}

bool Channel::hasKey() const
{
    return (!_key.empty());
}

bool Channel::checkKey(const std::string &key) const
{
    return (_key == key);
}

void Channel::setKey(const std::string &key)
{
    _key = key;
}

void Channel::removeKey()
{
    _key.clear();
}

bool Channel::hasLimit() const
{
    return (_userLimit > 0);
}

void Channel::setUserLimit(size_t limit)
{
    _userLimit = limit;
}

void Channel::removeUserLimit()
{
    _userLimit = 0;
}

bool Channel::isFull() const
{
    if (!hasLimit())
        return (false);

    return (_members.size() >= _userLimit);
}

bool Channel::addMember(Client *client)
{
    return (_members.insert(client).second);
}

bool Channel::removeMember(Client *client)
{
    if (_members.erase(client) == 0)
        return (false);

    _operators.erase(client);
    _invited.erase(client);

    return (true);
}

bool Channel::hasMember(Client *client) const
{
    return (_members.find(client) != _members.end());
}

const std::set<Client*> &Channel::getMembers() const
{
    return (_members);
}

bool Channel::addOperator(Client *client)
{
    if (!hasMember(client))
        return (false);

    return (_operators.insert(client).second);
}

bool Channel::removeOperator(Client *client)
{
    return (_operators.erase(client) > 0);
}

bool Channel::isOperator(Client *client) const
{
    return (_operators.find(client) != _operators.end());
}

void Channel::invite(Client *client)
{
    if (!hasMember(client))
        _invited.insert(client);
}

void Channel::removeInvite(Client *client)
{
    _invited.erase(client);
}

bool Channel::isInvited(Client *client) const
{
    return (_invited.find(client) != _invited.end());
}

size_t Channel::getMemberCount() const
{
    return (_members.size());
}

bool Channel::empty() const
{
    return (_members.empty());
}

void Channel::broadcast(const std::string &message)
{
    std::set<Client *>::iterator it;

    for (it = _members.begin(); it != _members.end(); ++it)
        (*it)->sendMessage(message);
}

