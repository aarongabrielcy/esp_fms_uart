idf_component_register(
    SRCS "main.cpp" "main.cpp"
    PRIV_REQUIRES spi_flash
    INCLUDE_DIRS "."
    REQUIRES uart serial power
)
