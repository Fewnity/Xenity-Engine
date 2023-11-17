# Socket

## Methods
| Definition | Description |
|-|-|
void SendData(const std::string& text) | Send data
std::string GetIncommingData() | Return recieved data during this frame (needs to be called every frame to avoid missing data)
