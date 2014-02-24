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
	
	--player light
	if IsLit(id) then
		Unlit(id, 1)
	elseif IsUnlit(id) then
		Lit(id, 1)
	end
	
	--sun
	if IsLit(201) then
		Unlit(201, 10)
	elseif IsUnlit(201) then
		Lit(201, 10)
	end
end

--platform_start script--
-------------------------

function activated_platform_start(id)
	SetContinous(24, true)
end
	
function deactivated_platform_start(id)
	SetContinous(24, false)
end


--btnStair script--
-------------------
function load_temporaryFlowers(id)
	StartUpdate()
end


platformTimer = 0;
stairdown = true
function update_temporaryFlowers(id, dt)
	if IsDown(id) then
		platformTimer = platformTimer + dt
		--Print(platformTimer)
		if stairdown then
			platformTimer = 0
			MoveToTarget(26)
			MoveToTarget(27)
			MoveToTarget(28)
			MoveToTarget(29)
			MoveToTarget(30)
			stairdown = false
		end
	else
		if not stairdown then
			MoveToSpawn(26)
			MoveToSpawn(27)
			MoveToSpawn(28)
			MoveToSpawn(29)
			MoveToSpawn(30)
			platformTimer = 0
			stairdown = true
		end
	end
	if platformTimer > 6 then
		ButtonUp(id)
		platformTimer = 0
	end
end

function load_fallingPlatform(id)
	StartUpdate()
end

standingTimer = 0
downTimer = 0
function update_fallingPlatform(id, dt)	
	if FallingPlatform(id) then
		downTimer = 0
		standingTimer = standingTimer +dt;
		if standingTimer > 1.5 then
			MoveToTarget(id)
			standingTimer = 0
		end	
	end
	if not OnAPlatform() then
		downTimer = downTimer + dt
		--Print(downTimer)
		if downTimer > 6 then
			downTimer = 0
			MoveToSpawn(5)
			MoveToSpawn(6)
			MoveToSpawn(7)
			MoveToSpawn(8)
		end
	end
end



--aiPush script--
-----------------

function load_aiPush(id)
	Print("AI loaded")
	
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


--TestAI script--
-----------------

function load_testAI(id)
	StartUpdate()
end

throwtime = 0
currtarget = 511;
function update_testAI(id, dt)
	SetTarget(id, currtarget)
	if InRange(id, currtarget, 4) then
		if currtarget == 511 then
			currtarget = 512
		else
			currtarget = 511
		end
	end
end

function skit(id, dt)
	PickUp(id)
	if not CanThrow(id, 98713) then
		SetTarget(id, 98713)
	else
		if InRange(id, player, 60) then
			throwtime = throwtime + dt
			SetTarget(id, player)
			if throwtime > 2 then
				Throw(id)
				throwtime = 0
			end
		else --patrulera
			SetTarget(id, currtarget)
			if InRange(id, currtarget, 4) then
				if currtarget == 511 then
					currtarget = 512
				else
					currtarget = 511
				end
			end
		end
	end
end


--ballonPop script--
--------------------

function activated_balloon(id)
	Print("Touched")
	if IsActivator(id, "Throwable") then
		Print("POP!")
		Pop(id)
	end
end


--Goal script--
---------------

function cutscene_Goal()
	Print("cutscene_goal")
	x, y, z = GetCameraPos()
	yaw, pitch = GetYawPitch()

	AddPoint(500, 190, 51.8, 90, 45, 2)
	AddPoint(545, 140, 51.8, 90, 30, 2)
end

function update_Goal(id)
	--Print("update_Goal")
	if not CutSceneIsPlaying() then
		ChangeLevel(0)
	end
end

function activated_Goal(id)
	Print("activated_Goal")

	CutScenePlay("Goal")
	StartUpdate()

end



