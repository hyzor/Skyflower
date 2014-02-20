function cutscene_intro()
	Print("intro")
	SetCamera(0, 0, 0, 0 ,45)
	AddPoint(-80, 80, -30 ,45 ,0, 0.5)
end


function loaded()
	if CompletedLevelCount() == 1 then
		StartMorph(10,1,0,0,0.2)
		CutScenePlay("intro")
	end	
end

function activated_Level(id)
	ChangeLevel(4)
end