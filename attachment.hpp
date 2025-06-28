#ifndef KEN_ATTACHMENT_H
#define KEN_ATTACHMENT_H

#include <string>
#include <memory>
#include <typeinfo>
#include <vector>
#include <functional>
#include <any>
#include <cassert>

namespace ken {

class AttachmentSet {
    struct TypeInfo {
        size_t type;
        std::function<std::shared_ptr<void>(const std::any&)> caster;
    };

    struct Attachment {
        std::string name;
        std::any data;
        TypeInfo type_info;
    };

    using Storage = std::vector<Attachment>;
    Storage storage_;

public:
    class Iterator {
        Storage::const_iterator it_;
    public:
        explicit Iterator(Storage::const_iterator it) : it_(it) {}
        
        const std::string& name() const { return it_->name; }
        
        template<typename T>
        bool is() const { return it_->type_info.type == typeid(T).hash_code(); }
        
        template<typename T>
        std::shared_ptr<T> get() const {
            if (!is<T>()) return nullptr;
            return std::static_pointer_cast<T>(it_->type_info.caster(it_->data));
        }
        
        Iterator& operator++() { ++it_; return *this; }
        bool operator!=(const Iterator& other) const { return it_ != other.it_; }
    };

    bool empty() const { return storage_.empty(); }
    size_t size() const { return storage_.size(); }

    bool contains(const std::string& name) const {
        return std::find_if(storage_.begin(), storage_.end(),
            [&name](const auto& a) { return a.name == name; }) != storage_.end();
    }

    template<typename T>
    bool is(const std::string& name) const {
        auto it = find(name);
        return it != storage_.end() && it->type_info.type == typeid(T).hash_code();
    }

    template<typename T>
    std::shared_ptr<T> get(const std::string& name) const {
        auto it = find(name);
        if (it == storage_.end() || it->type_info.type != typeid(T).hash_code()) {
            return nullptr;
        }
        return std::static_pointer_cast<T>(it->type_info.caster(it->data));
    }

    template<typename T>
    void set(const std::string& name, std::shared_ptr<T> data) {
        TypeInfo type_info{
            typeid(T).hash_code(),
            [](const std::any& a) { return std::any_cast<std::shared_ptr<T>>(a); }
        };

        auto it = find(name);
        if (it != storage_.end()) {
            it->data = data;
            it->type_info = type_info;
        } else {
            storage_.push_back({name, data, type_info});
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
    Storage::const_iterator find(const std::string& name) const {
        return std::find_if(storage_.begin(), storage_.end(),
            [&name](const auto& a) { return a.name == name; });
    }

    Storage::iterator find(const std::string& name) {
        return std::find_if(storage_.begin(), storage_.end(),
            [&name](const auto& a) { return a.name == name; });
    }
};

} // namespace ken

#endif // KEN_ATTACHMENT_H
