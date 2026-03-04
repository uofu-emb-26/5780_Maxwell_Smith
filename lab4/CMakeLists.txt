# Add C source files here.
add_executable(lab4
    Src/lab4.c
    Src/stm32f0xx_hal_msp.c
    Src/stm32f0xx_it.c
)

# Add the map file to the list of files to be removed with 'clean' target
set_target_properties(lab4 PROPERTIES ADDITIONAL_CLEAN_FILES lab4.map)

# Add user defined include paths
target_include_directories(lab4 PRIVATE
    Inc
)

# Add project symbols (macros)
target_compile_definitions(lab4 PRIVATE
)

# target_include_directories(STM32_Drivers BEFORE PRIVATE
#     ${CMAKE_CURRENT_LIST_DIR}/Inc
# )

# target_compile_definitions(STM32_Drivers PRIVATE
#     HAL_USART_MODULE_ENABLED
# )

# Add linked libraries
target_link_libraries(lab4
    STM32_Drivers
)

flash_target(lab4)
