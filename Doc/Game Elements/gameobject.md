# GameObject

## General Methods
| Definition | Description |
|-|-|
std::shared_ptr\<GameObject> CreateGameObject() | Create a GameObject with the default name (GameObject)
std::shared_ptr\<GameObject> CreateGameObject(const std::string& name) | Create a GameObject
std::shared_ptr\<GameObject> FindGameObjectByName(const std::string& name) | Find a GameObject with a name
std::shared_ptr\<GameObject> FindGameObjectById(const uint64_t id) | Find a GameObject with an id
std::shared_ptr\<Component> FindComponentById(const uint64_t id) | Find a component with an id
std::vector\<std::shared_ptr\<GameObject>> FindGameObjectsByName(const std::string& name) | Find GameObjects with a name

## Methods
| Definition | Description |
|-|-|
void AddChild(const std::shared_ptr\<GameObject>& gameObject) | Add a child to the GameObject
void SetParent(const std::shared_ptr\<GameObject>& gameObject) | Set GameObject's parent
std::shared_ptr\<T> AddComponent() | Add a component of T type
std::shared_ptr\<T> GetComponent() | Get a component of T type
bool GetActive()  | Get if the GameObject is marked as active
bool GetLocalActive()  | Get if the GameObject is active based on his parents
void SetActive(const bool active)  | Set GameObject as active or not
int GetChildrenCount()  | Get children count
int GetComponentCount() | Get component count
std::shared_ptr\<Transform> GetTransform() | Get transform

## Members
| Name | Type | Description |
|-|-|-|
name | std::string | GameObject's name
children | std::vector\<std::weak_ptr\<GameObject>> | GameObject's children list
components | std::vector\<std::shared_ptr\<Component>> | GameObject's components list
