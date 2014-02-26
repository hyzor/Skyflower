player = 1 --player id

function loaded()
	Lit(195,1)
	Lit(196,1)
	Unlit(197,1)
	Unlit(198,1)
	Unlit(199,1)
	Unlit(200,1)
	Unlit(201,1)
	Unlit(202,1)
	Unlit(203,1)
	Unlit(204,1)
	Unlit(205,1)
	Unlit(206,1)
	Unlit(207,1)
	Unlit(208,1)
	--CutScenePlay("intro")
end


--Intro script--
----------------

function cutscene_intro()
-- syntax: AddPoint(x, y, z, yaw, pitch, speed)
	SetCamera(0, 0, 0, 0, 45)
	AddPoint(140, 100, 20, -90, 30, 1)
	AddPoint(-140, 40, 5, -90, -10, 1.5)
	AddPoint(-120, 80, -120, 0, 20, 2)
	AddPoint(80, 30, 20, -110, 15, 1)	
end

--player script--
-----------------

function load_player(id)
	StartUpdate()
end

function update_player(id, dt)
	--PushAll(id)
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
			MoveToTarget(22)
			MoveToTarget(27)
			MoveToTarget(28)
			MoveToTarget(29)
			MoveToTarget(30)
			Lit(202,1)
			Lit(203,1)
			stairdown = false
		end
	else
		if not stairdown then
			MoveToSpawn(22)
			MoveToSpawn(27)
			MoveToSpawn(28)
			MoveToSpawn(29)
			MoveToSpawn(30)
			Unlit(202,1)
			Unlit(203,1)
			platformTimer = 0
			stairdown = true
		end
	end
	if platformTimer > 20 then
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
		if standingTimer > 0.8 then
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
	--if IsActivator(id, "Throwable") then
		Print("mmmm you touched me")
		Pop(id)
		MoveToTarget(46)
		Lit(206,3)
		Lit(207,3)
		Lit(208,3)
	--end
end


--Goal script--
---------------

function cutscene_Goal()
	Print("cutscene_goal")
	--x, y, z = GetCameraPos()
	--yaw, pitch = GetYawPitch()

	AddPoint(-1000, 100, 707, 90, 45, 2)
	AddPoint(-1200, 200, 707, 110, 20, 2)
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

function load_blinkingLights(id)
	StartUpdate()
end

timer = 0
function update_blinkingLights(id, dt)
	if IsActivated(id) then
		timer = timer + dt
		if timer > 0 and timer < 1 then
			Lit(197, 1)
		elseif timer > 1 and timer < 2 then
			Unlit(197, 1)
		elseif timer > 2 and timer < 3 then
			Lit(198, 1)
		elseif timer > 3 and timer < 4 then
			Unlit(198,1)
		elseif timer > 4 then
			timer = 0
		end
	end
end

function activated_light199(id)
	if IsActivated(id) then
		Lit(199,1)
	end
end

function activated_light200(id)
	if IsActivated(id) then
		Lit(200,1)
		Lit(204,1)
		Lit(205,1)
	end
end

function load_boxPuzzle(id)
	StartUpdate()
end

finished = false
failed = false
function update_boxPuzzle(id, dt)
	-- blue, yellow, red
	if BoxOnButton(56, 59) and BoxOnButton(66, 58) and BoxOnButton(64, 60) then
		if not finished then
			finished = true
			failed = false
			MoveToTarget(id)
			Lit(201,1)
		end
	elseif finished and not failed then
		MoveToSpawn(id)
		Unlit(201,1)
		finished = false
		failed = true
	end
end



