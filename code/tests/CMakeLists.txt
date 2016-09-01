#-------------------------------------------------------------------------------
# Tests
#-------------------------------------------------------------------------------


SET (TEST_PROJECTS

	#faudiotestsimple
	#testscript
	#drasa3testui
	#forestviewer
	#inputtest
	jobstest
	mathtest
	#multiplayerconnectiontest
	#PS3 only?
	#spumathtest
	#stresstestvieweropenclose
	testaddon
	testbase
	testfoundation
	testrender
	testtoolkit
	testframe2
	#texturestreamingtest_win32
	#viewerstresstest
	zipstresstest
	ziptest
	)
	
IF(MSVC)
LIST(APPEND TEST_PROJECTS testwin32)
ENDIF()
IF(N_BUILD_BULLET)
LIST(APPEND TEST_PROJECTS

		#bullet_collision_simple
		#bullet_dynamics_simple
	)
	
ENDIF()
IF(N_BUILD_HAVOK)
LIST(APPEND TEST_PROJECTS

		havok_helloworld
		havok_load_asset
		havok_player_test
	)
	
ENDIF()
	
SUBDIRS(${TEST_PROJECTS})

