player = 1 --player id

function loaded()
	CutScenePlay("intro")
end


--Intro script--
----------------

function cutscene_intro()
-- syntax: AddPoint(x, y, z, yaw, pitch, speed)
	SetCamera(0, 0, 0, 0, 45)
	AddPoint(0, 100, -130, 0, 45, 1)
	AddPoint(180, 150, -120, 0, 45, 2)
	AddPoint(110, 100, 100, 90, 20, 2)
	AddPoint(320, 130, 80, 0, 30, 1)
	AddPoint(450, 200, -50, 0, 45, 1)
	AddPoint(380, 150, 60, 90, 20, 1)
	AddPoint(750, 250, 60, -90, 20, 1)
	AddPoint(60, 30, 0, -90, 20, 1)
end

--player script--
-----------------

function load_player(id)
	StartUpdate()
end

function update_player(id, dt)
	PushAll(id)
end

--platform_start script--
-------------------------
first = true
function activated_platform_start(id)
	if first then
		PlayFinishedSound(id)
		first = false
	end
	SetContinous(24, true)
end
	
function deactivated_platform_start(id)
	SetContinous(24, false)
end


--btnStair script--
-------------------
function load_btnStair(id)
	StartUpdate()
end


stairdown = true
function update_btnStair(id, dt)
	if IsDown(id) then
		if stairdown then
			MoveToTarget(20)
			MoveToTarget(80)
			MoveToTarget(23)
			MoveToTarget(10)
			MoveToTarget(90)
			stairdown = false
		end
	else
		if not stairdown then
			MoveToSpawn(20)
			MoveToSpawn(80)
			MoveToSpawn(23)
			MoveToSpawn(10)
			MoveToSpawn(90)
			stairdown = true
		end
	end
end



--aiPush script--
-----------------

function load_aiPush(id)
	StartUpdate()
end

rush = true
rushtime = 0
scaredtime = 0
function update_aiPush(id, dt)
	
	if IsTouching(id, 16) then
		Jump(id)
	end

	if not stairdown then
		SetTarget(id,16)
		if IsActivated(16) and IsStanding(id, 16) then
			Jump(id)
		end
	else
		
		if rush then
			SetTarget(id, player)
			
			if InRange(id, player, 15) then
				SetSpeed(id, 50)
				if CanPush(id, player) then
					Push(id, player)
					rush = false
					scaredtime = 0
				elseif rushtime > 2 then
					rush = false
					scaredtime = 0
				end
			
				rushtime = rushtime + dt
			else
				SetSpeed(id, 10)
			end
		else
			SetTarget(id, player, 30)
			SetSpeed(id, 30)
			if scaredtime > 4 then
				rush = true
				rushtime = 0
			end
			
			scaredtime = scaredtime + dt
		end
	end
	
end

--ballonPop script--
--------------------

function activated_balloon(id)
	if IsActivator(id, "Throwable") then
		Print("POP!")
		Pop(id)
	end
end


--Goal script--
---------------

function cutscene_Goal()
	x, y, z = GetCameraPos()
	yaw, pitch = GetYawPitch()

	AddPoint(500, 190, 51.8, 90, 45, 2)
	AddPoint(545, 140, 51.8, 90, 30, 2)
end

function update_Goal(id)
	if not CutSceneIsPlaying() then
		ChangeLevel(0)
	end
end

function activated_Goal(id)
	CutScenePlay("Goal")
	StartUpdate()

end



