cmake_minimum_required(VERSION 3.16)
project(pcb-tracer)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

option(USE_OPENGL "Enable OpenGL support" OFF)

if(USE_OPENGL)
    add_definitions(-DUSE_OPENGL)
    find_package(Qt6 COMPONENTS OpenGL REQUIRED)
    find_package(Qt6 COMPONENTS OpenGLWidgets REQUIRED)
endif()

add_executable(pcb-tracer
	main.cpp
	Sidebar.cpp
	Sidebar.h
	MainWindow.cpp
	MainWindow.h
    CommunicationHub.h
	Editor.cpp
	Editor.h
	TrackDrawingTool.cpp
	TrackDrawingTool.h
	ComponentDrawingTool.cpp
	ComponentDrawingTool.h
	NotesTool.cpp
	NotesTool.h
	IEditorTool.h
	ImageLayer.cpp
	ImageLayer.h
	Node.cpp
	Node.h
	Link.cpp
	Link.h
	Component.cpp
	Component.h
	Config.cpp
	Config.h
	ColorBox.cpp
	ColorBox.h
	GuideTool.cpp
    GuideTool.h
    SceneLoader.cpp
    SceneLoader.h
	ZoomableGraphicsView.cpp
	ZoomableGraphicsView.h
	QGraphicsItemLayer.cpp
	QGraphicsItemLayer.h
	SceneLoaderBinary.cpp
	SceneLoaderBinary.h
	ConfigDialog.cpp
	ConfigDialog.h
	ConnectionAnalyzer.cpp
	ConnectionAnalyzer.h
	actions/AddTrack.cpp
	actions/AddTrack.h
	actions/MoveNode.cpp
	actions/MoveNode.h
	actions/DeleteTrack.cpp
	actions/DeleteTrack.h
	actions/AssignSideToTrack.cpp
	actions/AssignSideToTrack.h
	actions/AddComponent.cpp
	actions/AddComponent.h
)

qt_add_resources(pcb-tracer "RESOURCES"
    PREFIX "/"
    FILES resources.qrc
)

target_link_libraries(pcb-tracer PRIVATE Qt6::Widgets)

if(USE_OPENGL)
    target_link_libraries(pcb_tracer PRIVATE Qt6::OpenGLWidgets)
endif()

if(WIN32)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
	
		set_target_properties(pcb-tracer PROPERTIES LINK_FLAGS "-mwindows")

		set(DEPLOY_DIR "${CMAKE_SOURCE_DIR}/deploy")

		add_custom_command(TARGET pcb-tracer POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E make_directory "${DEPLOY_DIR}"
			COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:pcb-tracer>" "${DEPLOY_DIR}/"
		)
	find_program(WINDEPLOYQT_EXECUTABLE NAMES windeployqt6 windeployqt-qt6 windeployqt REQUIRED)

		# Run windeployqt after build
	add_custom_command(TARGET pcb-tracer POST_BUILD
		COMMAND ${WINDEPLOYQT_EXECUTABLE}
			--compiler-runtime
			--no-translations
			--no-system-d3d-compiler
			"${DEPLOY_DIR}/pcb-tracer.exe"
		COMMENT "Running windeployqt..."
	)

		# Copy missing dlls
		set(RUNTIME_DLLS 
			"libmd4c.dll"
			"libdouble-conversion.dll"
			"libicuin78.dll"
			"libicuuc78.dll"
			"libpcre2-16-0.dll"
			"libicudt78.dll"
			"libwinpthread-1.dll"
		)
		foreach(DLL ${RUNTIME_DLLS})
			find_program(DLL_PATH NAMES ${DLL})
			if(DLL_PATH)
				add_custom_command(TARGET pcb-tracer POST_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy_if_different "${DLL_PATH}" "${DEPLOY_DIR}/"
				)
			endif()
			unset(DLL_PATH CACHE)
		endforeach()
    endif()
endif()
 
install(TARGETS pcb-tracer DESTINATION bin)