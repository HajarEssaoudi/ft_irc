#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>

#include "Client.hpp"

class Channel
{
    private:
        std::string _name;
        std::string _topic;

        std::string _key;
        size_t      _userLimit;

        bool _inviteOnly;
        bool _topicRestricted;

        std::set<Client*> _members;
        std::set<Client*> _operators;
        std::set<Client*> _invited;

    public:
        // Orthodox Canonical Form
        // Channel();
        Channel(const std::string &name);
        Channel(const Channel &other);
        Channel &operator=(const Channel &other);
        ~Channel();

        // Getters
        const std::string &getName() const;
        const std::string &getTopic() const;
        const std::string &getKey() const;

        bool isInviteOnly() const;
        bool isTopicRestricted() const;
        bool hasKey() const;
        bool isFull() const;
        bool hasLimit() const;
        bool checkKey(const std::string &key) const;
        size_t getUserLimit() const;

        // Setters
        void setTopic(const std::string &topic);
        void setInviteOnly(bool value);
        void setTopicRestricted(bool value);
        void setKey(const std::string &key);
        void removeKey();
        void setUserLimit(size_t limit);
        void removeUserLimit();

        // Members
        bool addMember(Client *client);
        bool removeMember(Client *client);
        bool hasMember(Client *client) const;

        const std::set<Client *> &getMembers() const;

        // Operators
        bool addOperator(Client *client);
        bool removeOperator(Client *client);
        bool isOperator(Client *client) const;

        const std::set<Client *> &getOperators() const;

        // Invited users
        void invite(Client *client);
        void removeInvite(Client *client);
        bool isInvited(Client *client) const;

        // Utility
        size_t getMemberCount() const;
        bool empty() const;
};

#endif
