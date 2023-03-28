project(assets)

set(
	ASSETS
	fonts/actionj.ttf
	fonts/cartoon-relief.ttf
	fonts/instruction.otf
	fonts/typography-ties.ttf
	gamecontrollerdb.txt
	img/block1.png
	img/block10.png
	img/block11.png
	img/block12.png
	img/block2.png
	img/block3.png
	img/block4.png
	img/block5.png
	img/block6.png
	img/block7.png
	img/block8.png
	img/block9.png
	sound/clean.wav
	sound/drop.wav
	sound/move.wav
	sound/startup.wav
)

foreach (a ${ASSETS})
	cmake_path(GET a FILENAME filename)
	cmake_path(GET a PARENT_PATH parent)
	cmake_path(REPLACE_EXTENSION filename "h" OUTPUT_VARIABLE header)

	if (NOT parent)
		set(parent ".")
	endif ()

	set(output ${assets_BINARY_DIR}/${parent}/${header})
	set(input ${assets_SOURCE_DIR}/${a})

	add_custom_command(
		OUTPUT ${output}
		COMMAND
			${CMAKE_COMMAND} -E make_directory ${assets_BINARY_DIR}/${parent}
		COMMAND
			$<TARGET_FILE:bcc> ${input} assets_${a} > ${output}
		COMMENT
			"Generating ${header}"
		DEPENDS $<TARGET_FILE:bcc> ${input}
	)

	list(APPEND files ${output})
endforeach ()

add_library(libassets INTERFACE ${files})
target_include_directories(libassets INTERFACE $<BUILD_INTERFACE:${assets_BINARY_DIR}>)
