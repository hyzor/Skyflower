function cutscene_intro()
	Print("intro")
	SetCamera(0, 0, 0, 0 ,45)
	AddPoint(-80, 80, -30 ,45 ,0, 0.5)
end


function loaded()
	CutScenePlay("intro")
end

function activated_Level(id)
	ChangeLevel(4)
end