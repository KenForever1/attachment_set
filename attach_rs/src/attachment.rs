use std::any::{Any, TypeId};
use std::collections::{HashMap, hash_map};
use std::rc::Rc;
// use std::sync::Arc;

#[derive(Default)]
pub struct AttachmentSet {
    data: HashMap<String, (TypeId, Rc<dyn Any>)>,
}

// // 只需替换Rc为Arc并添加Send+Sync约束
// pub struct ThreadSafeAttachmentSet {
//     data: HashMap<String, (TypeId, Arc<dyn Any + Send + Sync>)>
// }

/// 附件集合的迭代器项
pub struct IterItem<'a> {
    name: &'a str,
    type_id: TypeId,
    value: &'a Rc<dyn Any>,
}

impl<'a> IterItem<'a> {
    pub fn name(&self) -> &str {
        self.name
    }

    pub fn is<T: Any>(&self) -> bool {
        self.type_id == TypeId::of::<T>()
    }

    pub fn get<T: Any>(&self) -> Option<&T> {
        if self.is::<T>() {
            self.value.downcast_ref::<T>()
        } else {
            None
        }
    }
}

/// 附件集合的可变迭代器项
pub struct IterMutItem<'a> {
    name: &'a str,
    type_id: TypeId,
    value: &'a mut Rc<dyn Any>,
}

impl<'a> IterMutItem<'a> {
    pub fn name(&self) -> &str {
        self.name
    }

    pub fn is<T: Any>(&self) -> bool {
        self.type_id == TypeId::of::<T>()
    }

    pub fn get<T: Any>(&self) -> Option<&T> {
        if self.is::<T>() {
            self.value.downcast_ref::<T>()
        } else {
            None
        }
    }

    pub fn get_mut<T: Any>(&mut self) -> Option<&mut T> {
        if self.is::<T>() {
            Rc::get_mut(self.value).and_then(|v| v.downcast_mut::<T>())
        } else {
            None
        }
    }
}

/// 附件集合的迭代器
pub struct Iter<'a> {
    inner: hash_map::Iter<'a, String, (TypeId, Rc<dyn Any>)>,
}

impl<'a> Iterator for Iter<'a> {
    type Item = IterItem<'a>;

    fn next(&mut self) -> Option<Self::Item> {
        self.inner.next().map(|(name, (tid, value))| IterItem {
            name: name.as_str(),
            type_id: *tid,
            value,
        })
    }
}

/// 附件集合的可变迭代器
pub struct IterMut<'a> {
    inner: hash_map::IterMut<'a, String, (TypeId, Rc<dyn Any>)>,
}

impl<'a> Iterator for IterMut<'a> {
    type Item = IterMutItem<'a>;

    fn next(&mut self) -> Option<Self::Item> {
        self.inner.next().map(|(name, (tid, value))| IterMutItem {
            name: name.as_str(),
            type_id: *tid,
            value,
        })
    }
}

impl AttachmentSet {
    pub fn new() -> Self {
        Self {
            data: HashMap::new(),
        }
    }

    pub fn is_empty(&self) -> bool {
        self.data.is_empty()
    }

    pub fn len(&self) -> usize {
        self.data.len()
    }

    pub fn contains(&self, name: &str) -> bool {
        self.data.contains_key(name)
    }

    pub fn contains_type<T: Any>(&self, name: &str) -> bool {
        self.data.get(name)
            .map(|(tid, _)| *tid == TypeId::of::<T>())
            .unwrap_or(false)
    }

    pub fn get<T: Any>(&self, name: &str) -> Option<Rc<T>> {
        self.data.get(name)
            .and_then(|(tid, value)| {
                if *tid == TypeId::of::<T>() {
                    value.clone().downcast::<T>().ok()
                } else {
                    None
                }
            })
    }

    pub fn set<T: Any>(&mut self, name: String, value: T) {
        self.data.insert(name, (TypeId::of::<T>(), Rc::new(value)));
    }

    pub fn remove(&mut self, name: &str) -> bool {
        self.data.remove(name).is_some()
    }

    pub fn clear(&mut self) {
        self.data.clear();
    }

    pub fn merge(&mut self, other: &AttachmentSet) {
        for (k, v) in &other.data {
            self.data.insert(k.clone(), (v.0, v.1.clone()));
        }
    }

    pub fn iter(&self) -> Iter {
        Iter {
            inner: self.data.iter()
        }
    }

    pub fn iter_mut(&mut self) -> IterMut {
        IterMut {
            inner: self.data.iter_mut()
        }
    }
}

impl<'a> IntoIterator for &'a AttachmentSet {
    type Item = IterItem<'a>;
    type IntoIter = Iter<'a>;

    fn into_iter(self) -> Self::IntoIter {
        self.iter()
    }
}

impl<'a> IntoIterator for &'a mut AttachmentSet {
    type Item = IterMutItem<'a>;
    type IntoIter = IterMut<'a>;

    fn into_iter(self) -> Self::IntoIter {
        self.iter_mut()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_basic_operations() {
        let mut attachments = AttachmentSet::new();
        assert!(attachments.is_empty());

        attachments.set("name".to_string(), "Alice".to_string());
        assert_eq!(attachments.get::<String>("name").unwrap().as_ref(), "Alice");
        assert_eq!(attachments.len(), 1);

        assert!(attachments.contains_type::<String>("name"));
        assert!(!attachments.contains_type::<i32>("name"));

        assert!(attachments.remove("name"));
        assert!(!attachments.contains("name"));
    }

    #[test]
    fn test_iterator() {
        let mut attachments = AttachmentSet::new();
        attachments.set("str".to_string(), "hello".to_string());
        attachments.set("num".to_string(), 42i32);

        let mut names = Vec::new();
        for item in attachments.iter() {
            names.push(item.name().to_string());
            if item.is::<i32>() {
                assert_eq!(item.get::<i32>().unwrap(), &42);
            }
        }
        names.sort();
        assert_eq!(names, vec!["num", "str"]);

        for mut item in attachments.iter_mut() {
            if item.is::<i32>() {
                *item.get_mut::<i32>().unwrap() += 1;
            }
        }
        assert_eq!(*attachments.get::<i32>("num").unwrap(), 43);
    }

    struct MyStruct {
        a: i32,
        b: String,
    }
    
    #[test]
    fn test_self_define_struct() {

        let mut attachments = AttachmentSet::new();
        attachments.set("my_struct".to_string(), MyStruct{a: 1, b: "Alice".to_string()});

        // 获取
        if let Some(my_struct) = attachments.get::<MyStruct>("my_struct") {
            assert_eq!(my_struct.a, 1);
            assert_eq!(my_struct.b, "Alice".to_string());
        }
    }
}
