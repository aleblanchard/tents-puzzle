link_directories(${CMAKE_SOURCE_DIR}) 
cmake_minimum_required(VERSION 3.5)

project(game_tents C)
enable_testing()
include(CTest)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -g")

add_library(game game.c game_aux.c game_ext.c game_tools.c)

add_executable(game_text game_text.c)
target_link_libraries(game_text game)

target_include_directories(game PUBLIC ${CMAKE_SOURCE_DIR})

add_executable(game_random game_random.c)
target_link_libraries(game_random game)

add_executable(game_solve game_solve.c)
target_link_libraries(game_solve game)

add_executable(game_test_fclaus game_test_fclaus.c)
add_executable(game_test_aleblanchard game_test_aleblanchard.c)
add_executable(game_test_tlaurans game_test_tlaurans.c)
add_executable(game_test_gabongrain game_test_gabongrain.c)

target_link_libraries(game_test_fclaus game)
target_link_libraries(game_test_aleblanchard game)
target_link_libraries(game_test_tlaurans game)
target_link_libraries(game_test_gabongrain game)

## find SDL2
include(sdl2.cmake)
message(STATUS "SDL2 include dir: ${SDL2_ALL_INC}")
message(STATUS "SDL2 all libraries: ${SDL2_ALL_LIBS}")

## compilation rules
include_directories(${SDL2_ALL_INC})
add_executable(game_sdl main.c model.c)
target_link_libraries(game_sdl game ${SDL2_ALL_LIBS} m)

## copy useful ressources in the build directory
file(COPY res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


#TESTS ALEXIS
add_test(test_aleblanchard_dummy ./game_test_aleblanchard dummy)
add_test(test_aleblanchard_new_empty ./game_test_aleblanchard test_game_new_empty)
add_test(test_aleblanchard_equal ./game_test_aleblanchard test_game_equal)
add_test(test_aleblanchard_copy ./game_test_aleblanchard test_game_copy)
add_test(test_aleblanchard_set_square ./game_test_aleblanchard test_game_set_square)
add_test(test_aleblanchard_delete ./game_test_aleblanchard test_game_delete)
add_test(test_aleblanchard_new ./game_test_aleblanchard test_game_new)
#Tests v2
add_test(test_aleblanchard_new_empty_v2 ./game_test_aleblanchard test_game_new_empty_v2)
add_test(test_aleblanchard_equal_v2 ./game_test_aleblanchard test_game_equal_v2)
add_test(test_aleblanchard_copy_v2 ./game_test_aleblanchard test_game_copy_v2)
add_test(test_aleblanchard_set_square_v2 ./game_test_aleblanchard test_game_set_square_v2)
add_test(test_aleblanchard_delete_v2 ./game_test_aleblanchard test_game_delete_v2)
add_test(test_aleblanchard_new_ext_v2 ./game_test_aleblanchard test_game_new_ext_v2)

#TESTS GAUTHIER
add_test(test_gabongrain_dummy ./game_test_gabongrain dummy)
add_test(test_gabongrain_game_fill_grass ./game_test_gabongrain test_game_fill_grass)
add_test(test_gabongrain_won ./game_test_gabongrain test_game_won)
add_test(test_gabongrain_check_move ./game_test_gabongrain test_game_check_move)
add_test(test_gabongrain_play_move ./game_test_gabongrain test_game_play_move)
add_test(test_gabongrain_get_square ./game_test_gabongrain test_game_get_square)
add_test(test_gabongrain_load ./game_test_gabongrain test_game_load)
add_test(test_gabongrain_save ./game_test_gabongrain test_game_save)


#TESTS THOMAS
add_test(test_tlaurans_dummy ./game_test_tlaurans dummy)
add_test(test_tlaurans_default_solution ./game_test_tlaurans default_solution)
add_test(test_tlaurans_game_is_neigh ./game_test_tlaurans game_is_neigh)
add_test(test_tlaurans_game_get_adjacent_square ./game_test_tlaurans game_get_adjacent_square)
add_test(test_tlaurans_game_default ./game_test_tlaurans game_default)
add_test(test_tlaurans_game_print ./game_test_tlaurans game_print)
add_test(test_tlaurans_game_restart ./game_test_tlaurans game_restart)
add_test(test_tlaurans_undo_redo ./game_test_tlaurans undo_redo)


#TESTS FCLAUS
add_test(test__fclaus_test_game_play_move_complementaire ./game_test_fclaus test_game_play_move_complementaire)
add_test(test__fclaus_game_nb_neigh_wrapping ./game_test_fclaus test_game_nb_neigh_wrapping)
add_test(test__fclaus_game_nb_neigh ./game_test_fclaus test_game_nb_neigh)
add_test(test_fclaus_bords_wrapping ./game_test_fclaus test_bords_wrapping)
add_test(test_fclaus_undo ./game_test_fclaus test_undo)
add_test(test_fclaus_redo ./game_test_fclaus test_redo)
add_test(test_fclaus_total ./game_test_fclaus test_total)
add_test(test_fclaus_ext ./game_test_fclaus test_ext)
add_test(test_fclaus_undo_ecrase ./game_test_fclaus test_undo_ecrase)
add_test(test_fclaus_dummy ./game_test_fclaus dummy)
add_test(test_fclaus_ecriture_row ./game_test_fclaus test_ecriture)
add_test(test_fclaus_effet_de_bord_row ./game_test_fclaus test_effet_de_bord)
add_test(test_fclaus_reecriture_row ./game_test_fclaus test_reecriture)
add_test(test_fclaus_reecriture_meme_val ./game_test_fclaus test_reecriture_meme_valeur)
add_test(test_fclaus_bornes_row ./game_test_fclaus test_bornes)
add_test(test_fclaus_miseajour_row ./game_test_fclaus test_miseajour)
add_test(test_fclaus_col_ecriture ./game_test_fclaus test_col_ecriture)
add_test(test_fclaus_col_effet_de_bord ./game_test_fclaus test_col_effet_de_bord)
add_test(test_fclaus_col_reecriture ./game_test_fclaus test_col_reecriture)
add_test(test_fclaus_col_reecrit_meme_val ./game_test_fclaus test_col_reecriture_meme_valeur)
add_test(test_fclaus_col_bornes ./game_test_fclaus test_col_bornes)
add_test(test_fclaus_col_miseajour ./game_test_fclaus test_col_miseajour)
add_test(test_fclaus_row_vide ./game_test_fclaus test_row_vide)
add_test(test_fclaus_row_apres_set ./game_test_fclaus test_apres_set)
add_test(test_fclaus_row_bornes ./game_test_fclaus test_get_row_bornes)
add_test(test_fclaus_all_empty ./game_test_fclaus test_all_empty)
add_test(test_fclaus_all_apres_set ./game_test_fclaus test_all_apres_set)
add_test(test_fclaus_current_row_vide ./game_test_fclaus test_current_row_vide)
add_test(test_fclaus_current_row_apres_set ./game_test_fclaus test_current_row_apres_set)
add_test(test_fclaus_current_row_bornes ./game_test_fclaus test_get_current_row_bornes)
add_test(test_fclaus_current_row_effet ./game_test_fclaus test_current_row_effet_de_bord)
add_test(test_fclaus_current_col_vide ./game_test_fclaus test_current_col_vide)
add_test(test_fclaus_current_col_apres_set ./game_test_fclaus test_current_col_apres_set)
add_test(test_fclaus_current_col_bornes ./game_test_fclaus test_get_current_col_bornes)
add_test(test_fclaus_current_row_effet_de_bord ./game_test_fclaus test_current_row_effet_de_bord)
add_test(test_fclaus_current_all_vide ./game_test_fclaus test_current_all_vide)
add_test(test_fclaus_current_all_apres_set ./game_test_fclaus test_current_all_apres_set)
add_test(test_fclaus_current_all_miseajour ./game_test_fclaus test_current_all_miseajour)
