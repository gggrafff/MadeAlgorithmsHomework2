cmake_minimum_required (VERSION 3.13)

if(NOT ADDITIONAL_FUNCTIONS_INCLUDED)
	include(Logger.cmake)

	# Additional functions

	function(add_link_directories TARGET_NAME DIRECTORIES)
		log(INFO "Setting link directories for target ${TARGET_NAME}: ;${DIRECTORIES}")
		target_link_directories(${TARGET_NAME} PRIVATE ${DIRECTORIES})
	endfunction()

	function(add_link_libraries TARGET_NAME LIBRARIES)
		log(INFO "Link target ${TARGET_NAME} with libraries: ;${LIBRARIES}")
		target_link_libraries(${TARGET_NAME} ${LIBRARIES})
	endfunction()

	function(add_include_directories TARGET_NAME DIRECTORIES)
		log(INFO "Setting include directories for target ${TARGET_NAME}: ;${DIRECTORIES}")
		target_include_directories(${TARGET_NAME} PRIVATE ${DIRECTORIES})
	endfunction()

	function(identify_target_platform)
		if(${CMAKE_GENERATOR_PLATFORM})
			log(DEBUG "CMAKE_GENERATOR_PLATFORM is defined.")
			set(TARGET_PLATFORM ${CMAKE_GENERATOR_PLATFORM} PARENT_SCOPE)
		else()
			if(${CMAKE_VS_PLATFORM_NAME})
				log(DEBUG "CMAKE_VS_PLATFORM_NAME is defined.")
				set(TARGET_PLATFORM ${CMAKE_VS_PLATFORM_NAME} PARENT_SCOPE)
			else()
				if(NOT ${CMAKE_CL_64} EQUAL "")
					log(DEBUG "CMAKE_CL_64 is defined.")
					if(${CMAKE_CL_64})
						set(TARGET_PLATFORM x64 PARENT_SCOPE)
					else()
						set(TARGET_PLATFORM x86 PARENT_SCOPE)
					endif()
				else()
					log(DEBUG "TARGET_PLATFORM identify by compiler name.")
					if(${CMAKE_CXX_COMPILER} MATCHES 64)
						set(TARGET_PLATFORM x64 PARENT_SCOPE)
					else()
						set(TARGET_PLATFORM x86 PARENT_SCOPE)
					endif()
				endif()
			endif()
		endif()
	endfunction()

	function(identify_library_postfix)
		if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
			set(LIBRARY_POSTFIX "d" PARENT_SCOPE)
		else()
			set(LIBRARY_POSTFIX "" PARENT_SCOPE)
		endif()
	endfunction()

	function(link_to_name LINK)
		log(DEBUG "Link to name for ${LINK}")
		# string(MD5 HASH_NAME ${LINK})
		get_filename_component(HASH_NAME "${LINK}" NAME_WE)
		set(HASH_NAME ${HASH_NAME} PARENT_SCOPE)
	endfunction()

	function(download_from_git LINK)
		link_to_name(${LINK})
		log(DEBUG "Downloading project name: ${HASH_NAME}")
		if(NOT ${HASH_NAME}_DOWNLOADED)
			if(${LINK} MATCHES git)
				file(WRITE CMakeLists.txt.in "
				# This is auto generated file.
				cmake_minimum_required (VERSION 3.13)
				project (${HASH_NAME}-download NONE)

				include(ExternalProject)
				ExternalProject_Add(${HASH_NAME}
					GIT_REPOSITORY ${LINK}
					GIT_TAG master
					DOWNLOAD_DIR \"${CMAKE_BINARY_DIR}/${HASH_NAME}-download\"
					SOURCE_DIR \"${CMAKE_BINARY_DIR}/${HASH_NAME}-src\"
					BINARY_DIR \"${CMAKE_BINARY_DIR}/${HASH_NAME}-build\"
					CONFIGURE_COMMAND \"\"
					BUILD_COMMAND \"\"
					INSTALL_COMMAND \"\"
					TEST_COMMAND \"\"
				)
				")
			else()
				file(WRITE CMakeLists.txt.in "
				# This is auto generated file.
				cmake_minimum_required (VERSION 3.13)
				project (${HASH_NAME}-download NONE)

				include(ExternalProject)
				ExternalProject_Add(${HASH_NAME}
					URL ${LINK}
					DOWNLOAD_DIR \"${CMAKE_BINARY_DIR}/${HASH_NAME}-download\"
					SOURCE_DIR \"${CMAKE_BINARY_DIR}/${HASH_NAME}-src\"
					BINARY_DIR \"${CMAKE_BINARY_DIR}/${HASH_NAME}-build\"
					CONFIGURE_COMMAND \"\"
					BUILD_COMMAND \"\"
					INSTALL_COMMAND \"\"
					TEST_COMMAND \"\"
				)
				")
			endif()
			configure_file("CMakeLists.txt.in" "${CMAKE_BINARY_DIR}/${HASH_NAME}-download/CMakeLists.txt")
			execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" . WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/${HASH_NAME}-download")
			execute_process(COMMAND "${CMAKE_COMMAND}" --build . WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/${HASH_NAME}-download")
			set(${HASH_NAME}_DOWNLOADED 1 PARENT_SCOPE)
		else()
			log(DEBUG "${HASH_NAME} is already downloaded.")
		endif()
	endfunction()

	function(add_external_library LINK)
		link_to_name(${LINK})
		if(NOT ${HASH_NAME}_LINKED)
			log(DEBUG "Link library ${HASH_NAME}")
			if(NOT ${HASH_NAME}_DOWNLOADED)
				if(EXISTS "${DEPENDENCIES_PATH}/${HASH_NAME}-src")
					log(DEBUG "${HASH_NAME} available locally.")
					add_subdirectory(
						"${DEPENDENCIES_PATH}/${HASH_NAME}-src"
						"${DEPENDENCIES_PATH}/${HASH_NAME}-build"
					)
					set(${HASH_NAME}_DOWNLOADED 1 PARENT_SCOPE)
				else()

						download_from_git(${LINK})
						add_subdirectory(
							"${CMAKE_BINARY_DIR}/${HASH_NAME}-src"
							"${CMAKE_BINARY_DIR}/${HASH_NAME}-build"
						)
				endif()
			endif()
			set(${HASH_NAME}_LINKED 1 PARENT_SCOPE)
		else()
			log(DEBUG "${HASH_NAME} is already linked.")
		endif()
	endfunction()

	function(add_gtest LINK)
		set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
		set(gtest_disable_pthreads ON CACHE BOOL "" FORCE)
		add_external_library(${LINK})
		link_to_name(${LINK})
		set(${HASH_NAME}_DOWNLOADED 1 PARENT_SCOPE)
	endfunction()

	function(add_boost)
		log(DEBUG "Set default parameters BOOST_URL and BOOST_URL_SHA256.")
		set(BOOST_URL "https://dl.bintray.com/boostorg/release/1.71.0/source/boost_1_71_0.tar.bz2" CACHE STRING "Boost download URL")
		set(BOOST_URL_SHA256 "d73a8da01e8bf8c7eda40b4c84915071a8c8a0df4a6734537ddde4a8580524ee" CACHE STRING "Boost download URL SHA256 checksum")

		log(DEBUG "Parse optional arguments.")
		set(extra_macro_args ${ARGN})
		list(LENGTH extra_macro_args num_extra_args)
		if(${num_extra_args} EQUAL 2)
			list(GET extra_macro_args 0 BOOST_URL)
			list(GET extra_macro_args 1 BOOST_URL_SHA256)
		endif()

		log(DEBUG "BOOST_URL is ${BOOST_URL}")
		log(DEBUG "BOOST_URL_SHA256 is ${BOOST_URL_SHA256}")
		link_to_name("${BOOST_URL}")

		if(${BOOST_URL} MATCHES git)
			log(WARNING "We recommend that you specify a link to the zip archive for ${HASH_NAME}.")
		endif()
		
		log(DEBUG "Boost project name: ${HASH_NAME}")
		if(NOT ${HASH_NAME}_DOWNLOADED)
			set(BOOSTCMAKE_GIT "https://github.com/Orphis/boost-cmake.git")
			add_external_library(${BOOSTCMAKE_GIT})
			set(${HASH_NAME}_DOWNLOADED 1 PARENT_SCOPE)
		else()
			log(DEBUG "${HASH_NAME} is already downloaded.")
		endif()

		set(Boost_INCLUDE_DIR ${CMAKE_BINARY_DIR}/_deps/boost-src PARENT_SCOPE)
		set(Boost_SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/boost-src PARENT_SCOPE)
		set(boost_INCLUDE_DIR ${CMAKE_BINARY_DIR}/_deps/boost-src PARENT_SCOPE)
		set(boost_SOURCE_DIR ${CMAKE_BINARY_DIR}/_deps/boost-src PARENT_SCOPE)
		set(BOOST_ROOT ${CMAKE_BINARY_DIR}/_deps/boost-src PARENT_SCOPE)
		set(BOOST_INCLUDEDIR ${CMAKE_BINARY_DIR}/_deps/boost-src PARENT_SCOPE)
		set(BOOST_LIBRARYDIR ${CMAKE_BINARY_DIR}/_deps/boost-src/libs PARENT_SCOPE)
		set(ENV{BOOST_ROOT} ${CMAKE_BINARY_DIR}/_deps/boost-src PARENT_SCOPE)
		set(ENV{BOOST_INCLUDEDIR} ${CMAKE_BINARY_DIR}/_deps/boost-src PARENT_SCOPE)
		set(ENV{BOOST_LIBRARYDIR} ${CMAKE_BINARY_DIR}/_deps/boost-src/libs PARENT_SCOPE)
	endfunction()

	function(print_all_properties)
		get_cmake_property(_variableNames VARIABLES)
		list(SORT _variableNames)
		foreach(_variableName ${_variableNames})
			message(STATUS "${_variableName} = ${${_variableName}}")
		endforeach()
	endfunction()

	set(ADDITIONAL_FUNCTIONS_INCLUDED true)
else()
	log(DEBUG "File already included. ")
endif()