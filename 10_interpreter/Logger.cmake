cmake_minimum_required (VERSION 3.13)

if(NOT LOGGER_INCLUDED)

	# Log configuration
	set(MAX_LOG_LEVEL DEFAULT)  # ERROR, WARNING, INFO, DEBUG or DEFAULT

	function(log_level_to_int LOG_LEVEL)
		if(${LOG_LEVEL} EQUAL ERROR)
			set(RETURNED_VALUE 1 PARENT_SCOPE)
		elseif(${LOG_LEVEL} EQUAL WARNING)
			set(RETURNED_VALUE 2 PARENT_SCOPE)
		elseif(${LOG_LEVEL} EQUAL INFO)
			set(RETURNED_VALUE 3 PARENT_SCOPE)
		elseif(${LOG_LEVEL} EQUAL DEBUG)
			set(RETURNED_VALUE 4 PARENT_SCOPE)
		else()
			if(${CMAKE_BUILD_TYPE} MATCHES Debug)
				set(RETURNED_VALUE 4 PARENT_SCOPE)  # default log level is DEBUG
			else()
				set(RETURNED_VALUE 3 PARENT_SCOPE)  # default log level is INFO
			endif()
		endif()
	endfunction()

	function(log LEVEL MESSAGES)
		set(IS_FIRST true)
		log_level_to_int(${MAX_LOG_LEVEL})
		set(MAX_LEVEL ${RETURNED_VALUE})
		log_level_to_int(${LEVEL})
		set(CURRENT_LEVEL ${RETURNED_VALUE})
		if(NOT CURRENT_LEVEL GREATER MAX_LEVEL)
			foreach(MESSAGE ${MESSAGES})
				if(${IS_FIRST})
					message("LOG[${LEVEL}]: ${MESSAGE}")
					set(IS_FIRST false)
				else()
					message("\t\t\t\t${MESSAGE}")
				endif()
			endforeach()
		endif()
	endfunction()

	set(LOGGER_INCLUDED true)
else()
	log(DEBUG "File already included. ")
endif()