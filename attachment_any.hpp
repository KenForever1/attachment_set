#ifndef KEN_ANY_ATTACHMENT_H
#define KEN_ANY_ATTACHMENT_H

#include <any>
#include <string>
#include <vector>
#include <typeinfo>
#include <algorithm>
#include <memory>

namespace ken {

class AnyAttachmentSet {
    struct NamedAny {
        std::string name;
        std::any data;
    };

    std::vector<NamedAny> storage_;

public:
    class Iterator {
        std::vector<NamedAny>::const_iterator it_;
    public:
        explicit Iterator(std::vector<NamedAny>::const_iterator it) : it_(it) {}
        
        const std::string& name() const { return it_->name; }
        
        template<typename T>
        bool is() const {
            return it_->data.type() == typeid(std::shared_ptr<T>);
        }
        
        template<typename T>
        std::shared_ptr<T> get() const {
            try {
                return std::any_cast<std::shared_ptr<T>>(it_->data);
            } catch (const std::bad_any_cast&) {
                return nullptr;
            }
        }
        
        Iterator& operator++() { ++it_; return *this; }
        bool operator!=(const Iterator& other) const { return it_ != other.it_; }
    };

    bool empty() const { return storage_.empty(); }
    size_t size() const { return storage_.size(); }

    bool contains(const std::string& name) const {
        return find(name) != storage_.end();
    }

    template<typename T>
    bool is(const std::string& name) const {
        auto it = find(name);
        return it != storage_.end() && it->data.type() == typeid(std::shared_ptr<T>);
    }

    template<typename T>
    std::shared_ptr<T> get(const std::string& name) const {
        auto it = find(name);
        if (it == storage_.end()) return nullptr;
        
        try {
            return std::any_cast<std::shared_ptr<T>>(it->data);
        } catch (const std::bad_any_cast&) {
            return nullptr;
        }
    }

    template<typename T>
    void set(const std::string& name, std::shared_ptr<T> data) {
        auto it = find(name);
        if (it != storage_.end()) {
            it->data = data;
        } else {
            storage_.push_back({name, data});
        }
    }

    void remove(const std::string& name) {
        storage_.erase(std::remove_if(storage_.begin(), storage_.end(),
            [&name](const auto& a) { return a.name == name; }), storage_.end());
    }

    void clear() { storage_.clear(); }

    Iterator begin() const { return Iterator(storage_.begin()); }
    Iterator end() const { return Iterator(storage_.end()); }

private:
    std::vector<NamedAny>::const_iterator find(const std::string& name) const {
        return std::find_if(storage_.begin(), storage_.end(),
            [&name](const auto& a) { return a.name == name; });
    }

    std::vector<NamedAny>::iterator find(const std::string& name) {
        return std::find_if(storage_.begin(), storage_.end(),
            [&name](const auto& a) { return a.name == name; });
    }
};

} // namespace ken

#endif // KEN_ANY_ATTACHMENT_H