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
	Unlit(209,1)
	CutScenePlay("intro")
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
	PushAll(id)
end

--platform_start script--
-------------------------

function activated_platform_start(id)
	SetContinous(24, true)
end
	
function deactivated_platform_start(id)
	SetContinous(24, false)
end


--temporaryFlowers script--
-------------------
function load_temporaryFlowers(id)
	StartUpdate()
end

activateAI = false
function activated_temporaryFlowers(id)
	activateAI = true
end


platformTimer = 0;
stairdown = true
function update_temporaryFlowers(id, dt)
	if IsDown(id) then
		platformTimer = platformTimer + dt
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
	if platformTimer > 10 then
		ButtonUp(id)
		platformTimer = 0
	end
end

function load_fallingPlatform5(id)
	StartUpdate()
end

standingTimer5 = 0
downTimer5 = 0
movingDown5 = false
function update_fallingPlatform5(id, dt)	
	if FallingPlatform(id) and not movingDown5 then
		downTimer5 = 0
		standingTimer5 = standingTimer5 +dt;
		if standingTimer5 > 0.3 then
			MoveToTarget(id)
			standingTimer5 = 0
			movingDown5 = true
		end	
	end
	if not FallingPlatform(id) then
		downTimer5 = downTimer5 + dt
		standingTimer5 = 0
		if downTimer5 > 2 then
			downTimer5 = 0
			MoveToSpawn(id)
			movingDown5 = false
		end
	end
end

function load_fallingPlatform6(id)
	StartUpdate()
end

standingTimer6 = 0
downTimer6 = 0
movingDown6 = false
function update_fallingPlatform6(id, dt)	
	if FallingPlatform(id) and not movingDown6 then
		downTimer6 = 0
		standingTimer6 = standingTimer6 +dt;
		if standingTimer6 > 0.3 then
			MoveToTarget(id)
			standingTimer6 = 0
			movingDown6 = true
		end	
	end
	if not FallingPlatform(id) then
		downTimer6 = downTimer6 + dt
		standingTimer6 = 0
		if downTimer6 > 2 then
			downTimer6 = 0
			MoveToSpawn(id)
			movingDown6 = false
		end
	end
end

function load_fallingPlatform7(id)
	StartUpdate()
end

standingTimer7 = 0
downTimer7 = 0
movingDown7 = false
function update_fallingPlatform7(id, dt)	
	if FallingPlatform(id) and not movingDown7 then
		downTimer7 = 0
		standingTimer7 = standingTimer7 +dt;
		if standingTimer7 > 0.3 then
			MoveToTarget(id)
			standingTimer7 = 0
			movingDown7 = true
		end	
	end
	if not FallingPlatform(id) then
		downTimer7 = downTimer7 + dt
		standingTimer7 = 0
		if downTimer7 > 2 then
			downTimer7 = 0
			MoveToSpawn(id)
			movingDown7 = false
		end
	end
end

function load_fallingPlatform8(id)
	StartUpdate()
end

standingTimer8 = 0
downTimer8 = 0
movingDown8 = false
function update_fallingPlatform8(id, dt)	
	if FallingPlatform(id) and not movingDown8 then
		downTimer8 = 0
		standingTimer8 = standingTimer8 +dt;
		if standingTimer8 > 0.3 then
			MoveToTarget(id)
			standingTimer8 = 0
			movingDown8 = true
		end	
	end
	if not FallingPlatform(id) then
		downTimer8 = downTimer8 + dt
		standingTimer8 = 0
		if downTimer8 > 2 then
			downTimer8 = 0
			movingDown8 = false
			MoveToSpawn(id)
		end
	end
end



--ballonPop script--
--------------------

function activated_balloon(id)
	--if IsActivator(id, "Throwable") then
		Pop(id)
		MoveToTarget(46)
		Lit(206,3)
		Lit(207,3)
		Lit(208,3)
		Lit(209,3)
		PlayFinishedSound(id)
	--end
end


--Goal script--
---------------

function cutscene_Goal()
	--x, y, z = GetCameraPos()
	--yaw, pitch = GetYawPitch()

	AddPoint(-1000, 100, 707, 90, 45, 2)
	AddPoint(-1200, 200, 707, 110, 20, 2)
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

function load_blinkingLights(id)
	StartUpdate()
end

timer = 0
first = true
function update_blinkingLights(id, dt)
	if IsActivated(id) then
		if first then
			PlayFinishedSound(id)
			first = false
		end
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
spawning = false
soundTimer = 0
timer = 0
function update_boxPuzzle(id, dt)
	-- blue, yellow, red
	if not IsAlive(59) or not IsAlive(58) or not IsAlive(60) or not spawning then
		Respawn(1)
		Respawn(59)
		SetCanMoveBox(59)
		Respawn(58)
		SetCanMoveBox(58)
		Respawn(60)	
		SetCanMoveBox(60)		
		spawning = true
	end
	timer = timer + dt
	if timer > 4 then
		spawning = true
	end
	if OnButton(56, 59) and OnButton(66, 58) and OnButton(64, 60) then
		if not finished and soundTimer > 0.2 then
			finished = true
			failed = false
			MoveToTarget(id)
			Lit(201,1)
			PlayFinishedSound(id)
			soundTimer = 0
		end
		soundTimer = soundTimer + dt
	elseif finished and not failed then
		MoveToSpawn(id)
		Unlit(201,1)
		finished = false
		failed = true
	end
	if OnButton(56, 59) then
		SetCanNotMoveBox(59)
	else
		SetCanMoveBox(59)
	end
	if OnButton(66, 58) then
		SetCanNotMoveBox(58)
	else
		SetCanMoveBox(58)
	end
	if OnButton(64, 60) then
		SetCanNotMoveBox(60)
			else
		SetCanMoveBox(60)
	end
end

function load_puzzleCheckpoint(id)
	StartUpdate()
end

firstAtCheckpoint = true
function update_puzzleCheckpoint(id, dt)
	if IsActivated(id) and timer > 3then
		if firstAtCheckpoint then
			Lit(199,1)
			firstAtCheckpoint = false
		end
		if not finished then
		Respawn(59)
		Respawn(58)
		Respawn(60)	
		timer = 0
		end
	else
		timer = timer + dt
	end
end






--thrower script--
-----------------

heldtime = 0
function update_thrower(id, dt)
	if not CanThrow(id, 101) then
		if not IsHeld(101) then
			SetTarget(id, 101)
			PickUp(id, 101)
		else
			SetTarget(id, player, 100)
		end
	else
		SetTarget(id, player, 60)
		heldtime = heldtime + dt
		if InRange(id, player, 60) and heldtime > 1 then
			Throw(id, player)
			heldtime = 0
		end
	end
end

--pusher script--
-----------------

function update_pusher(id, dt)
	if not IsDizzy(player) and IsAlive(73) then
		SetSpeed(id, 30)
		if InRange(id, player, 40) then
			SetTarget(id, player)
			if CanPush(id, player) then
				Push(id, player)
			end
		else
			SetTarget(id, player, 60)
		end
	else
		SetSpeed(id, 40)
		SetTarget(id, player)
		if CanPush(id, player) then
			Push(id, player)
		end
	end
end




--thrower1 script--
-------------------

function load_thrower1(id)
	StartUpdate()
end

throw1JumpTime = 2
throw1ChangeTargetTime = 0
throw1Target = 0
function update_thrower1(id, dt)
	if activateAI then
		if not InRange(id, player, 130) then
			throw1JumpTime = throw1JumpTime - dt
			if throw1JumpTime < 0 then
				throw1JumpTime = math.random(1, 5)
				Jump(id)
			end
			
			throw1ChangeTargetTime = throw1ChangeTargetTime - dt
			if throw1ChangeTargetTime < 0 then
				throw1ChangeTargetTime = math.random(2, 7)
				throw1Target = math.random(102, 109)
			end
			
			SetTarget(id, throw1Target)
		else
			update_thrower(id, dt)
		end
	end
end


--pusher2 script--
------------------

function load_pusher2(id)
	StartUpdate()
end

push2JumpTime = 2
push2ChangeTargetTime = 0
push2Target = 0
function update_pusher2(id, dt)
	if activateAI then
		if not InRange(id, player, 130) then
			push2JumpTime = push2JumpTime - dt
			if push2JumpTime < 0 then
				push2JumpTime = math.random(1, 5)
				Jump(id)
			end
			
			push2ChangeTargetTime = push2ChangeTargetTime - dt
			if push2ChangeTargetTime < 0 then
				push2ChangeTargetTime = math.random(2, 7)
				push2Target = math.random(102, 109)
			end
			
			SetTarget(id, push2Target)
		else
			update_pusher(id, dt)
		end
	end
end


--pusher1 script--
------------------

function load_pusher1(id)
	StartUpdate()
end

push1JumpTime = 2
push1ChangeTargetTime = 0
push1Target = 0
function update_pusher1(id, dt)
	if activateAI then
		if not InRange(id, player, 130) then
			push1JumpTime = push1JumpTime - dt
			if push1JumpTime < 0 then
				push1JumpTime = math.random(1, 5)
				Jump(id)
			end
			
			push1ChangeTargetTime = push1ChangeTargetTime - dt
			if push1ChangeTargetTime < 0 then
				push1ChangeTargetTime = math.random(2, 7)
				push1Target = math.random(102, 109)
			end
			
			SetTarget(id, push1Target)
		else
			update_pusher(id, dt)
		end
	end
end



