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
	
	-- Load correct signs in the hubworld, depending on if the subworlds are completed or not. --
	
	-- 4 = ID of subworld 1 -- 	
	-- 536 = ID of the sign that indicates that the player hasn't finished subworld 1 --
	-- 537 = ID of the sign that indicates that the player has finished subworld 1 --
	if LevelIsCompleted(4) then
		SetActivated(536, false)
		SetActivated(537, true)
	else
		SetActivated(536, true)
		SetActivated(537, false)
	end
	
	-- 5 = ID of subworld 2 --
	-- 538 = ID of the sign that indicates that the player hasn't finished subworld 2 --
	-- 539 = ID of the sign that indicates that the player has finished subworld 2 --
	if LevelIsCompleted(5) then
		SetActivated(538, false)
		SetActivated(539, true)
	else
		SetActivated(538, false)
		SetActivated(539, true)
	end
end

function activated_Level(id)
	if id == 33 then
		ChangeLevel(4)
	elseif id == 34 then
		ChangeLevel(5)
	end
end