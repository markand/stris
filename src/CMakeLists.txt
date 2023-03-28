project(stris)

set(
	SOURCES
	${stris_SOURCE_DIR}/board.c
	${stris_SOURCE_DIR}/board.h
	${stris_SOURCE_DIR}/joy.c
	${stris_SOURCE_DIR}/joy.h
	${stris_SOURCE_DIR}/key.h
	${stris_SOURCE_DIR}/list.c
	${stris_SOURCE_DIR}/list.h
	${stris_SOURCE_DIR}/score.c
	${stris_SOURCE_DIR}/score.h
	${stris_SOURCE_DIR}/shape.c
	${stris_SOURCE_DIR}/shape.h
	${stris_SOURCE_DIR}/sound.c
	${stris_SOURCE_DIR}/sound.h
	${stris_SOURCE_DIR}/state-dead.c
	${stris_SOURCE_DIR}/state-dead.h
	${stris_SOURCE_DIR}/state-menu.c
	${stris_SOURCE_DIR}/state-menu.h
	${stris_SOURCE_DIR}/state-mode.c
	${stris_SOURCE_DIR}/state-mode.h
	${stris_SOURCE_DIR}/state-play.c
	${stris_SOURCE_DIR}/state-play.h
	${stris_SOURCE_DIR}/state-scores.c
	${stris_SOURCE_DIR}/state-scores.h
	${stris_SOURCE_DIR}/state-settings.c
	${stris_SOURCE_DIR}/state-settings.h
	${stris_SOURCE_DIR}/state-splash.c
	${stris_SOURCE_DIR}/state-splash.h
	${stris_SOURCE_DIR}/state.c
	${stris_SOURCE_DIR}/state.h
	${stris_SOURCE_DIR}/stris.6
	${stris_SOURCE_DIR}/stris.c
	${stris_SOURCE_DIR}/stris.h
	${stris_SOURCE_DIR}/sys.c
	${stris_SOURCE_DIR}/sys.h
	${stris_SOURCE_DIR}/tex.c
	${stris_SOURCE_DIR}/tex.h
	${stris_SOURCE_DIR}/ui.c
	${stris_SOURCE_DIR}/ui.h
	${stris_SOURCE_DIR}/util.c
	${stris_SOURCE_DIR}/util.h
)

add_executable(stris ${SOURCES})
target_link_libraries(
	stris
	libassets
	SDL3::SDL3
	SDL3_image::SDL3_image
	SDL3_mixer::SDL3_mixer
	SDL3_ttf::SDL3_ttf
)
target_compile_definitions(
	stris
	PRIVATE
		VARDIR="${CMAKE_INSTALL_FULL_LOCALSTATEDIR}"
)

if (MATH_LIBRARY)
	target_link_libraries(stris ${MATH_LIBRARY})
endif ()
