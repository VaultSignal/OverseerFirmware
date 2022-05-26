idf_component_register(
    SRCS "main.cpp" "src/NetworkClient.cpp" "src/WatcherController.cpp" "src/RadioReceiver.cpp"
    INCLUDE_DIRS "include"
    COMPONENT_EMBED_FILES "src/getpassword.html"
)

